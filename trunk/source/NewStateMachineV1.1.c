/***new implementation of state machine**********/
/*version 1.1:
-first we calculate the required transitions and store them in the bit of the global variable "EcatTransition"
-then for each transition: 1)we execute the init commands in the master lists (the "before slave" commands)
                           2)we execute the init commands in slaves lists (one command per slave)
						   
there was an error in the former version: first ALL init commands in ALL master's list were executed and then slaves init commands

for example: suppose we have two transition, IP and PS; in the former version it was executed pMaster->pIPInitCmd, then pMaster->pPSInitCmd 
and then ec_slave[i].more->pIPInit and ec_slave[i].more->pPSInit. but this order is worng.
the correct order of execution is pMaster->pIPInitCmd, ec_slave[i].more->pIPInit, pMaster->pPSInitCmd, ec_slave[i].more->pPSInit*/

uint16 EcatTransition=0; /* this variable must be golobal. it stores ALL required transition in theirs bit */


/*definition of transition masks. each mask set 1 bit of variable EcatTransition*/
#define ECAT_INITCMD_I_P                0x0001
#define ECAT_INITCMD_P_S                0x0002
#define ECAT_INITCMD_P_I                0x0004
#define ECAT_INITCMD_S_P                0x0008
#define ECAT_INITCMD_S_O                0x0010
#define ECAT_INITCMD_S_I                0x0020
#define ECAT_INITCMD_O_S                0x0040
#define ECAT_INITCMD_O_P                0x0080
#define ECAT_INITCMD_O_I                0x0100
#define ECAT_INITCMD_I_B                0x0200
#define ECAT_INITCMD_B_I                0x0400
#define ECAT_INITCMD_BEFORE             0x8000 //not needed any more

#define MASTER_CMD                      OxFFFF


///////////////////////////////////////////////////////////
//MasterStateMachine
//compares CurrentState and RequireSate and store in transition ALL the transitions needed to make currentState=ReqequesState
//@param[IN]  pMaster pointer to Maaster descriptor 
//[OUT] true if all transition have been executed

boolean MasterStateMachine (Ecmaster *pMaster)
{ 
  if (EcatTransition==0) //  if there's already at least one transition to be execute, skip
   {
     if(pMaster->m_currState!=pMaster->m_reqState)
	   {
	     switch (pMaster->m_currState)
		    {
			 case EC_STATE_INIT:
			       switch (pMaster->m_reqState)
				   {
				     case EC_STATE_INIT:
					      EcatTransition =0; //it's not necessary beacuse in this case CurrentState=EC_STATE_INIT=RequestState
						  break;
				     case EC_STATE_PRE_OP:
					      EcatTransition = ECAT_INITCMD_I_P;
						  break;
					case EC_STATE_SAFE_OP:
					      EcatTransition = ECAT_INITCMD_I_P|ECAT_INITCMD_P_S; //to go from init to safe-op we have to execute both IP and PS transitions
						  break;
					case EC_STATE_OPERATIONAL:
					      EcatTransition = ECAT_INITCMD_I_P|ECAT_INITCMD_P_S|ECAT_INITCMD_S_O;
						  break;	  
				   }
		     case EC_STATE_PRE_OP:
			       switch (pMaster->m_reqState)
				   {
				     case EC_STATE_INIT:
					      EcatTransition =ECAT_INITCMD_P_I; 
						  break;
				     case EC_STATE_PRE_OP:
					      EcatTransition = 0;
						  break;
					case EC_STATE_SAFE_OP:
					      EcatTransition = ECAT_INITCMD_P_S;
						  break;
					case EC_STATE_OPERATIONAL:
					      EcatTransition = ECAT_INITCMD_P_S|ECAT_INITCMD_S_O;
						  break;	  
				   }
			case EC_STATE_SAFE_OP:
			       switch (pMaster->m_reqState)
				   {
				     case EC_STATE_INIT:
					      EcatTransition =ECAT_INITCMD_S_I; 
						  break;
				     case EC_STATE_PRE_OP:
					      EcatTransition = ECAT_INITCMD_S_P; 
						  break;
					case EC_STATE_SAFE_OP:
					      EcatTransition = 0;
						  break;
					case EC_STATE_OPERATIONAL:
					      EcatTransition = ECAT_INITCMD_S_O;
						  break;	  
				   }
			case EC_STATE_OPERATIONAL:
			       switch (pMaster->m_reqState)
				   {
				     case EC_STATE_INIT:
					      EcatTransition =ECAT_INITCMD_O_I; 
						  break;
				     case EC_STATE_PRE_OP:
					      EcatTransition = ECAT_INITCMD_O_P; 
						  break;
					case EC_STATE_SAFE_OP:
					      EcatTransition = ECAT_INITCMD_O_S;
						  break;
					case EC_STATE_OPERATIONAL:
					      EcatTransition = 0;
						  break;
                    }
             default: 
			     EcatTransition=0;
				 break;					
			}
	    }
	 
    }
/* version 1.0
	if (MasterInitCmd(Master))
	{  //all master's init command are been executed; we can now execute the init command ofr the slaves
	   if (SlaveInitCmd())
	     //all init commands are been executed
		 return TRUE;
	}
 return (MasterTransition||SlaveTransition) ? FALSE : TRUE;   
}
   end version 1.0 */
   
  /* version 1.1*/
 if (MasterInitCmd(Master))
	{  	   
		 return TRUE;
	}
 return (EcatTransition ? FALSE : TRUE);   
} 
////////////////////////////////////////////////////////////////
//MasterInitCmd
// check the value of EcatTransition, pick the proper init command list and pass it to function "InitCmd, update the value of EcatTransition
//first, it executes all the transition with ECAT_INITCMD_BEFORE set to 1, then it call the SlaveInitCmd function 
//@param[IN]  pMaster pointer to Maaster descriptor
//@param[OUT] true if EcatTransition is 0 (all transitions has been executed)

boolean MasterInitCmd(Ecmaster *pMaster)
{
 if (EcatTransition&ECAT_INITCMD_BEFORE) //skip if there isn't master's init command
    {
	
	 if (EcatTransition&ECAT_INITCMD_I_P)
	    { 
          if(InitCmd(pMaster->pIPInitCmd, MASTER_CMD))	 //call the function that executes the Init Cmd passing the proper list; 
		    {                                //InitCmd return TRUE if all the command in the list has been executed 
            /*version 1.1 */											
			 if (SlaveInitCmd(ECAT_INITCMD_I_P)) //after the execution of the "before slave" init command, we can execute the init command store in
			                                     //slave's lists for this transition
			     EcatTransition&=!(ECAT_INITCMD_I_P); //all the command for this transition has been sent. we can reset the bit
			 else return FALSE;
			} 
		 else return FALSE; // skip everything else if we haven't finished yet with this transition
			
	    }
	if (EcatTransition&ECAT_INITCMD_P_S)
	    { 
          if(InitCmd(pMaster->pPSInitCmd, MASTER_CMD))	 
		    {                                
            /*version 1.1 */											
			 if (SlaveInitCmd(ECAT_INITCMD_P_S)) 
			 
			     EcatTransition&=!(ECAT_INITCMD_P_S); 
			 else return FALSE;
			} 
		  else return FALSE;
	    }
	if (EcatTransition&ECAT_INITCMD_P_I)
	    { 
          if(InitCmd(pMaster->pPIInitCmd, MASTER_CMD))	 
		    {                               
            /*version 1.1 */											
			 if (SlaveInitCmd(ECAT_INITCMD_P_I)) 
			 
			     EcatTransition&=!(ECAT_INITCMD_P_I); 
			 else return FALSE;
			} 
		  else return FALSE;
			
	    }
	if (EcatTransition&ECAT_INITCMD_S_P)
	    { 
          if(InitCmd(pMaster->pSPInitCmd, MASTER_CMD))	 
		    {                                
            /*version 1.1 */											
			 if (SlaveInitCmd(ECAT_INITCMD_S_P)) 
			 
			     EcatTransition&=!(ECAT_INITCMD_S_P); 
			 else return FALSE;
			} 
		  else return FALSE;
			
	    }
	if (EcatTransition&ECAT_INITCMD_S_O)
	    { 
          if(InitCmd(pMaster->pSOInitCmd, MASTER_CMD))	 
		    {                                
            /*version 1.1 */											
			 if (SlaveInitCmd(ECAT_INITCMD_S_O))
			 
			     EcatTransition&=!(ECAT_INITCMD_S_O); 
			 else return FALSE;
			} 
		 else return FALSE;
			
	    }
	if (EcatTransition&ECAT_INITCMD_S_I)
	    { 
          if(InitCmd(pMaster->pSIInitCmd, MASTER_CMD))	 
		    {                               
            /*version 1.1 */											
			 if (SlaveInitCmd(ECAT_INITCMD_S_I)) 
			     EcatTransition&=!(ECAT_INITCMD_S_I); 
			 else return FALSE;
			} 
		 else return FALSE;
			
	    }
	if (EcatTransition&ECAT_INITCMD_O_S)
	    { 
          if(InitCmd(pMaster->pSOInitCmd, MASTER_CMD))	 
		    {                                
            /*version 1.1 */											
			 if (SlaveInitCmd(ECAT_INITCMD_O_S)) 
			     EcatTransition&=!(ECAT_INITCMD_O_S); 
			 else return FALSE;
			} 
		 else return FALSE;
			
	    }
	if (EcatTransition&ECAT_INITCMD_O_P)
	    { 
          if(InitCmd(pMaster->pOPInitCmd, MASTER_CMD))	 
		    {                                
            /*version 1.1 */											
			 if (SlaveInitCmd(ECAT_INITCMD_O_P)) 
			     EcatTransition&=!(ECAT_INITCMD_O_P); 
			 else return FALSE;
			} 
		 else return FALSE;
			
	    }
    if (EcatTransition&ECAT_INITCMD_O_I)
	    { 
          if(InitCmd(pMaster->pOIInitCmd, MASTER_CMD))	 
		   {                                
            /*version 1.1 */											
			 if (SlaveInitCmd(ECAT_INITCMD_O_I)) 
			     EcatTransition&=!(ECAT_INITCMD_O_I); 
			 else return FALSE;
			} 
		 else return FALSE;
			 
	    }

   	
	return EcatTransition ? FALSE : TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//SlaveInitCmd
//execute the init command for the slaves
// return bSlaveReady (true if all the commands for the given transition are been executed)

boolean SlaveInitCmd(uint16 transition)
{
  boolean bSlaveReady=TRUE;
 
  int i=0;
 /*version 1.1*/
 
       if (transition==ECAT_INITCMD_I_P)
	    { 
		   for (i=0; i<ec_slavecount; i++)
            {
		      bSlaveReady&=InitCmd(ec_slave[i].more->pIPInit, i);
			}
             
	    }
	if (transition==ECAT_INITCMD_P_S)
	    { 
          for (i=0; i<ec_slavecount; i++)
            {
		      bSlaveReady&=InitCmd(ec_slave[i].more->pPSInit, i);
			}
             
			 
		}
	if (transition==ECAT_INITCMD_P_I)
	    { 
          for (i=0; i<ec_slavecount; i++)
            {
		      bSlaveReady&=InitCmd(ec_slave[i].more->pPIInit, i);
			}
              
	    }
	if (transition==ECAT_INITCMD_S_P)
	    { 
          for (i=0; i<ec_slavecount; i++)
            {
		      bSlaveReady&=InitCmd(ec_slave[i].more->pSPInit, i);
			}
             			
	    }
	if (transition==ECAT_INITCMD_S_O)
	    { 
          for (i=0; i<ec_slavecount; i++)
            {
		      bSlaveReady&=InitCmd(ec_slave[i].more->pSOInit, i);
			}
             
			
	    }
	if (transition==ECAT_INITCMD_S_I)
	    { 
          for (i=0; i<ec_slavecount; i++)
            {
		      bSlaveReady&=InitCmd(ec_slave[i].more->pSIInit, i);
			}
              
			
	    }
	if (transition==ECAT_INITCMD_O_S)
	    { 
          for (i=0; i<ec_slavecount; i++)
            {
		      bSlaveReady&=InitCmd(ec_slave[i].more->pOSInit, i);
			}
              
			
	    }
	if (transition==ECAT_INITCMD_O_P)
	    { 
          for (i=0; i<ec_slavecount; i++)
            {
		      bSlaveReady&=InitCmd(ec_slave[i].more->pOPInit, i);
			}
              
			
	    }
    if (transition==ECAT_INITCMD_O_I)
	    { 
          for (i=0; i<ec_slavecount; i++)
            {
		      bSlaveReady&=InitCmd(ec_slave[i].more->pOIInit, i);
			}
              			 
	    }

     return  bSlaveReady;
}


//////////////////////////////////////////////////////////////////////
//InitCmd
//@param [IN] IntiCmdList
//@param [IN] index of slave or MASTER_CMD
// return out if all command in the list are been executed

boolean InitCmd(InitCmdList *List, uint16 nSlave)
{
 uint16 i=0;
  
 if(nSlave==MASTER_CMD) // it's a master init command
   {
      
     if ( pMaster->m_cInitCmds < pMaster->initcmdCnt )
	    {
		//skip initcmd already sent
         while(i!=pMaster->m_cInitCmds)  
	         {List=List->next;
			  i++;}
          EcatCmdReq(...) // the same function to build frame
		 pMaster->m_cInitCmds++; 
        }
	 if ( pMaster->m_cInitCmds < pMaster->initcmdCnt )
          return FALSE;
     else
        {
		  pMaster->m_cInitCmds=INITCMD_INACTIVE;
		  return TRUE;
		 }	 
   
    }
else 
    {
      if ( ec_slave[nSlave].more->m_cInitCmds < ec_slave[nSlave].more->initcmdCnt )
	    {
		//skip initcmd already sent
         while(i!=ec_slave[nSlave].more->m_cInitCmds)  
	         {List=List->next;
			  i++;}
          EcatCmdReq(...) // the same function to build frame
		 ec_slave[nSlave].more->m_cInitCmds++; 
        }
	 if ( ec_slave[nSlave].more->m_cInitCmds < ec_slave[nSlave].more->initcmdCnt )
	    
          return FALSE;
     else
        {
		  ec_slave[nSlave].more->m_cInitCmds=INITCMD_INACTIVE;
		  return TRUE;
		 }	   
    } 
}
    

