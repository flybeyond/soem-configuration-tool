int GetTransition(Ecmaster *pMaster) //TODO: ADD B_I AND I_B TRANSITIONS
{ 
  int EcatTransition=0;
 
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
return EcatTransition;	 
   }
   
/////////////////////////////////////////////////////////////////////////////
boolean TransitionIP(Ecmaster *pMaster)
{
 uint16 slave, slavec, ssigen, configadr, nNumCmd;
 InitCmdList *SlaveList, *MasterList;
 
 /*send before_slave cmds*/
  nNumCmd=pMaster->nIPInitCmdCount;
  MasterList=pMaster->pIPInitCmd
    while(MasterList&&nNumCmd)
	{
	  EcatCmdReq(MasterList->InitCmd);
	  nNumCmd--;
	  MasterList=MasterList->nextCmd;
	}  
  /*config slaves*/
  for (slave=1; slave<=ec_slavecount; slave++)
    {
	 /*slave's init_cmds*/
	 SlaveList=ec_slaveMore[slave].pIPInit;
	  while(SlaveList)
	    {
		 EcatCmdReq(SlaveList->InitCmd);
	    SlaveList=SlaveList->nextCmd;
		 }
	/*update the ec_slave fields (second part; the first part has been done by XML parsing) this part is taken from ethercatconfig.c*/
     	/* Support DC? */
		configadr = ec_slave[slave].configadr;
		if ((etohs(ec_FPRDw(configadr, ECT_REG_ESCSUP, EC_TIMEOUTRET)) & 0x04) > 0)  
                ec_slave[slave].hasdc = TRUE;
            else
                ec_slave[slave].hasdc = FALSE;
		/* extract topology from DL status */		
            topology = etohs(ec_FPRDw(configadr, ECT_REG_DLSTAT, EC_TIMEOUTRET)); 
			h = 0; 
			b = 0;
            if ((topology & 0x0300) == 0x0200) /* port0 open and communication established */
            {
                h++;
				b |= 0x01;
            }
            if ((topology & 0x0c00) == 0x0800) /* port1 open and communication established */
            {
                h++;
				b |= 0x02;
            }
            if ((topology & 0x3000) == 0x2000) /* port2 open and communication established */
            {
                h++;
				b |= 0x04;
            }
            if ((topology & 0xc000) == 0x8000) /* port3 open and communication established */
            {
                h++;
				b |= 0x08;
            }
	       ec_slave[slave].topology = h;
			ec_slave[slave].activeports = b;
			/* 0=no links, not possible             */
            /* 1=1 link  , end of line              */
            /* 2=2 links , one before and one after */
            /* 3=3 links , split point              */
            /* 4=4 links , cross point              */
            /* search for parent */
            ec_slave[slave].parent = 0; /* parent is master */
            if (slave > 1)
            {
                topoc = 0; 
                slavec = slave - 1;
                do
                {
		            topology = ec_slave[slavec].topology;
                    if (topology == 1)
                        topoc--; /* endpoint found */
                    if (topology == 3)
                        topoc++; /* split found */
                    if (topology == 4)
                        topoc+=2; /* cross found */
                    if (((topoc >= 0) && (topology > 1)) ||
					    (slavec == 1)) /* parent found */
                    {
                        ec_slave[slave].parent = slavec;
                        slavec = 1;
                    }
					slavec--;
                }
                while (slavec > 0);
            }
			
		/* set default mailbox configuration if slave has mailbox */
			if (ec_slave[slave].mbx_l>0)
			{	
				ec_slave[slave].SMtype[0] = 1;
				ec_slave[slave].SMtype[1] = 2;
				ec_slave[slave].SMtype[2] = 3;
				ec_slave[slave].SMtype[3] = 4;
				ec_slave[slave].SM[0].StartAddr = htoes(ec_slave[slave].mbx_wo);
				ec_slave[slave].SM[0].SMlength = htoes(ec_slave[slave].mbx_l);
				ec_slave[slave].SM[0].SMflags = htoel(EC_DEFAULTMBXSM0);
				ec_slave[slave].SM[1].StartAddr = htoes(ec_slave[slave].mbx_ro);
				ec_slave[slave].SM[1].SMlength = htoes(ec_slave[slave].mbx_rl);
				ec_slave[slave].SM[1].SMflags = htoel(EC_DEFAULTMBXSM1);
				
			}
        ssigen = ec_siifind(slave, ECT_SII_GENERAL);
				/* SII general section */
				if (ssigen)
                {
					ec_slave[slave].CoEdetails = ec_siigetbyte(slave, ssigen + 0x07);
					ec_slave[slave].FoEdetails = ec_siigetbyte(slave, ssigen + 0x08);
					ec_slave[slave].EoEdetails = ec_siigetbyte(slave, ssigen + 0x09);
					ec_slave[slave].SoEdetails = ec_siigetbyte(slave, ssigen + 0x0a);
					if((ec_siigetbyte(slave, ssigen + 0x0d) & 0x02) > 0)
					{
						ec_slave[slave].blockLRW = 1;
						ec_slave[0].blockLRW++;						
					}	
					ec_slave[slave].Ebuscurrent = ec_siigetbyte(slave, ssigen + 0x0e);
					ec_slave[slave].Ebuscurrent += ec_siigetbyte(slave, ssigen + 0x0f) << 8;
					ec_slave[0].Ebuscurrent += ec_slave[slave].Ebuscurrent;
                }			
	}
  
 }


///////////////////////////////////////////////////////////////////////////
boolean MasterStateMachine(Ecmaster *pMaster) //TODO: ADD B_I AND I_B TRANSITIONS
{
 //int Ec_Transition=0;
      
	int  Ec_Transition=GetTransition(pMaster);
	
	/*transition_I_P*/
	 if (Ec_Transition&&ECAT_INITCMD_I_P)
	    if(TransitionIP(pMaster))
		     Ec_Transition&=!ECAT_INITCMD_I_P;
		else
		   return FALSE; //TODO: REPLACE WITH SOME ERR_CODE
		   
	/*transition_P_S*/	   
    if (Ec_Transition&&ECAT_INITCMD_P_S)
	    if(TransitionPS(pMaster))
		     Ec_Transition&=!ECAT_INITCMD_P_S;
		else
		   return FALSE; //TODO: REPLACE WITH SOME ERR_CODE
		   
	/*transition_S_O*/	   
    if (Ec_Transition&&ECAT_INITCMD_S_O)
	    if(TransitionSO(pMaster))
		     Ec_Transition&=!ECAT_INITCMD_S_O;
		else
		   return FALSE; //TODO: REPLACE WITH SOME ERR_CODE

	/*transition_P_I*/	   
    if (Ec_Transition&&ECAT_INITCMD_P_I)
	    if(TransitionPI(pMaster))
		     Ec_Transition&=!ECAT_INITCMD_P_I;
		else
		   return FALSE; //TODO: REPLACE WITH SOME ERR_CODE	 

   /*transition_S_I*/	   
    if (Ec_Transition&&ECAT_INITCMD_S_I)
	    if(TransitionSI(pMaster))
		     Ec_Transition&=!ECAT_INITCMD_S_I;
		else
		   return FALSE; //TODO: REPLACE WITH SOME ERR_CODE		

   /*transition_S_P*/	   
    if (Ec_Transition&&ECAT_INITCMD_S_P)
	    if(TransitionSP(pMaster))
		     Ec_Transition&=!ECAT_INITCMD_S_P;
		else
		   return FALSE; //TODO: REPLACE WITH SOME ERR_CODE	

   /*transition_O_I*/	   
    if (Ec_Transition&&ECAT_INITCMD_O_I)
	    if(TransitionOI(pMaster))
		     Ec_Transition&=!ECAT_INITCMD_O_I;
		else
		   return FALSE; //TODO: REPLACE WITH SOME ERR_CODE	
		   
    /*transition_O_P*/	   
    if (Ec_Transition&&ECAT_INITCMD_O_P)
	    if(TransitionOP(pMaster))
		     Ec_Transition&=!ECAT_INITCMD_O_P;
		else
		   return FALSE; //TODO: REPLACE WITH SOME ERR_CODE	

    /*transition_O_S*/	   
    if (Ec_Transition&&ECAT_INITCMD_O_S)
	    if(TransitionOS(pMaster))
		     Ec_Transition&=!ECAT_INITCMD_O_S;
		else
		   return FALSE; //TODO: REPLACE WITH SOME ERR_CODE		

return Ec_Transition ? FALSE:TRUE;		   
}




///////////////////////////////////////////////////////////////////////////////
///\brief Request EtherCAT master to change to the specified state.
void    MasterRequestState(uint16 state, EcMaster *pMaster)
{
    pMaster->m_currState=ec_readstate();// this function also update ec_slave[slave].state
	pMaster->m_reqState = state;
	    
 if (pMaster->m_reqState != pMaster->m_currState)
     MasterStateMachine(pMaster);
    
}

