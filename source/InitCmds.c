/*******************************************
 * SOEM Configuration tool
 *
 * File    : InitCmds.c
 * Version : 1.5
 * Date    : 19-04-2012
 * History :
 *          1.5, 19-04-2012, complete version for test
 *          1.4, 27-02-2012, global variables modified
 *			1.3, 24-02-2012, struct ec_slavet modified
 *          1.2, 24-01-2012, Improved readability
 *			1.1  10-01-2012, add a field in the pending frame to store the expected working counter for a given command (field "Cnt" in ENI XML file)
 *          1.0, 21-12-2011, Initial version 
****************************************************************/

#include "InitCmds.h"
#include "ethercattype.h"
#include "ethercatmain.h"
#include "ethercatbase.h"
#include "ethercatcoe.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
// define if debug printf is needed
//#define EC_DEBUG

#ifdef EC_DEBUG
#define EC_PRINT(...) do {} while (0)
#endif


int  Ec_Transition=0;
//////////////////////////////////////////////////////////////////////
/*
 brief: calculate ALL the transitions required to reach the requested state
 return the updated value of Ec_Transition
*/
//TODO: ADD B_I AND I_B TRANSITIONS
int GetTransition(void)
{ 
  int EcatTransition=0;
 
    
    	if(ec_slave[0].state!=ec_slaveMore[0].reqState) 
	   {
 		if (ec_slave[0].state==EC_STATE_NONE || ec_slave[0].state==EC_STATE_INIT) 
		    {
			          
			 
			      
 				   switch (ec_slaveMore[0].reqState) 
				   {
				     case EC_STATE_INIT:
					      ec_slave[0].state=EC_STATE_INIT;
			              ec_writestate(0);
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
				 }
		     
 			if (ec_slave[0].state==EC_STATE_PRE_OP) 
			      {
				   
 				  switch (ec_slaveMore[0].reqState) 
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
				  }
			
 		   if (ec_slave[0].state==EC_STATE_SAFE_OP)	
			  {
			      
 				   switch (ec_slaveMore[0].reqState) 
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
				 }
			
 		   if (ec_slave[0].state==EC_STATE_OPERATIONAL)	
			 {
			       
 				  switch (ec_slaveMore[0].reqState) 
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
				}
           }					
			
	    
return EcatTransition;	 
   }
//////////////////////////////////////////////////////////////////////////
/*
 brief: read the values of Synch Managers from the init commands and the store it 
 in the proper structures of ec_slave, so the slavelist is always in a consistent status
 */
  void set_sm(EcInitCmdDesc *CmdDesc, uint16 slave)
{
 int i=0;
 uint8 nSM,fSM;
 

   //is a broadcast?
    if (slave==0)
	{
	  for (i=1;i<=ec_slavecount;i++)
	    {
		 
		 memset(&(ec_slave[i].SM), 0, (CmdDesc->DataLength/8));
		 }
	 }
	else
	 {
	 
	   
       //first SM
	   fSM=(CmdDesc->ecHead.ADO-ECT_REG_SM0)/8;

 	  //how many SM (each SM is 8byte long)
	   nSM=(CmdDesc->DataLength)/8; 
	   if (nSM>1)
	    {
		 for (i=fSM; i<nSM; i++)
		  { memcpy(&(ec_slave[slave].SM[i]),&(CmdDesc->data[8*i]),sizeof(ec_smt));}
		 }
	   else  memcpy(&(ec_slave[slave].SM[fSM]),&(CmdDesc->data),sizeof(ec_smt));
	  }
} 
//////////////////////////////////////////////////////////////////////////
/*
 brief: read the values of FMMUs from the init commands and the store it 
 in the proper structures of ec_slave, so the slavelist is always in a consistent status
 */
  void set_fmmu(EcInitCmdDesc *CmdDesc, uint16 slave)
{
 int i=0;
 uint8 nFMMU,fFMMU;
 

   //is a broadcast?
    if (slave==0)
	{
	  for (i=1;i<=ec_slavecount;i++)
	    {
		 
		 memset(&(ec_slave[i].FMMU), 0, (CmdDesc->DataLength/8));
		 }
	 }
	else
	 {
	    //first FMMU
	    fFMMU=(CmdDesc->ecHead.ADO-ECT_REG_FMMU0)/16;
	   //how many FMMU (each FMMU is 16byte long)
	   nFMMU=(CmdDesc->DataLength)/16;
	   if(nFMMU>1)
	   {
	    for (i=fFMMU; i<=nFMMU; i++)
		 {memcpy(&(ec_slave[slave].FMMU[i]),&(CmdDesc->data[16*i]),sizeof(ec_fmmut));} 
	    }
		else memcpy(&(ec_slave[slave].FMMU[fFMMU]),&(CmdDesc->data),sizeof(ec_fmmut));
	 }
} 
  
  //////////////////////////////////////////////////////////////////////////
  /*
   brief: call the proper primitive of SOEM
   return wkc, -1 if error
  */
  
  int EcatCmdReq(EcInitCmdDesc *CmdDesc, uint32 *pData, uint16 Slave) 
  {
   int wkc;
   uint32 data;
   memcpy(&data,CmdDesc->data,sizeof(data));
   data=htoel(data);
   uint16 len;
   len=CmdDesc->DataLength;
     
    switch (CmdDesc->ecHead.command){ 
	 case EC_CMD_APRD:
	             wkc=ec_APRD(CmdDesc->ecHead.ADP,CmdDesc->ecHead.ADO,len,CmdDesc->data,EC_TIMEOUTSAFE); 
				 break;
     case EC_CMD_APWR:
	             wkc=ec_APWR(CmdDesc->ecHead.ADP,CmdDesc->ecHead.ADO,len,CmdDesc->data,EC_TIMEOUTSAFE); 
				 if(((CmdDesc->ecHead.ADO-ECT_REG_FMMU0)>=0)&&((CmdDesc->ecHead.ADO-ECT_REG_FMMU0)<=0x30))
				    {set_fmmu(CmdDesc,Slave);
					}
				 if(((CmdDesc->ecHead.ADO-ECT_REG_SM0)>=0)&&((CmdDesc->ecHead.ADO-ECT_REG_SM0)<=0x18))
				    {set_sm(CmdDesc,Slave);
					} 
				 break;
 	 case EC_CMD_APRW:
	             wkc=ec_ARMW(CmdDesc->ecHead.ADP,CmdDesc->ecHead.ADO,len,CmdDesc->data,EC_TIMEOUTSAFE);
				 break;
	 case EC_CMD_FPRD:
	             wkc=ec_FPRD(CmdDesc->ecHead.ADP,CmdDesc->ecHead.ADO,len,CmdDesc->data,EC_TIMEOUTSAFE);
				 break;
	 case EC_CMD_FPWR:
	             wkc=ec_FPWR(CmdDesc->ecHead.ADP,CmdDesc->ecHead.ADO,len,CmdDesc->data,EC_TIMEOUTSAFE);
				 if(((CmdDesc->ecHead.ADO-ECT_REG_FMMU0)>=0)&&((CmdDesc->ecHead.ADO-ECT_REG_FMMU0)<=0x30))
				    {set_fmmu(CmdDesc,Slave);
					}
				 if(((CmdDesc->ecHead.ADO-ECT_REG_SM0)>=0)&&((CmdDesc->ecHead.ADO-ECT_REG_SM0)<=0x18))
				    {set_sm(CmdDesc,Slave);
					} 
				 break;
	 //case EC_CMD_FPRW:
	             //not implemented in SOEM
				 //break; 
	 case EC_CMD_BRD:
	             wkc=ec_BRD(CmdDesc->ecHead.ADP,CmdDesc->ecHead.ADO,len,CmdDesc->data,EC_TIMEOUTSAFE);
				 break;
	 case EC_CMD_BWR:
	             wkc=ec_BWR(CmdDesc->ecHead.ADP,CmdDesc->ecHead.ADO,len,CmdDesc->data,EC_TIMEOUTSAFE);
				 if(CmdDesc->ecHead.ADO==ECT_REG_FMMU0)
				    {set_fmmu(CmdDesc,0);
					}
				 if(CmdDesc->ecHead.ADO==ECT_REG_SM0)
				    {set_sm(CmdDesc,0);
					} 
				 break;
	 //case EC_CMD_BRW:
	             //not implemented in SOEM
				// break; 
	 case EC_CMD_LRD:
	             wkc=ec_LRD(CmdDesc->ecHead.laddr,len,CmdDesc->data,EC_TIMEOUTSAFE);
				 break;
	 case EC_CMD_LWR:
	             wkc=ec_LWR(CmdDesc->ecHead.laddr,len,CmdDesc->data,EC_TIMEOUTSAFE);
				 break;
 	 case EC_CMD_LRW:
	             wkc=ec_LRW(CmdDesc->ecHead.laddr,len,CmdDesc->data,EC_TIMEOUTSAFE);
				 break;
	 case EC_CMD_ARMW:
	             wkc=ec_ARMW(CmdDesc->ecHead.ADP,CmdDesc->ecHead.ADO,len,CmdDesc->data,EC_TIMEOUTSAFE);
				 break;
	 case EC_CMD_FRMW: 
	             wkc=ec_FRMW(CmdDesc->ecHead.ADP,CmdDesc->ecHead.ADO,len,CmdDesc->data,EC_TIMEOUTSAFE);
				 break;
	  default:
	        wkc=-1;
	         break;
	 }
	 return wkc;
	} 
 /////////////////////////////////////////////////////////////////////////
/*
 brief: execute the init command.
   if fails, it tries to execute the command as many times as specified by the value of retry.
   return wkc, -1 if error
 */

int exec_cmd(EcInitCmdDesc *CmdDesc, uint16 slave)
{int wkc,cnt = 0; 
char i; 
 uint32 data=0;
 
 uint16 retry,estat;
 
   retry=CmdDesc->retries;
      if (CmdDesc->ecHead.ADO==0x502||CmdDesc->ecHead.ADO==0x508)
	    {
		  ec_eeprom2master(slave);
		 
         if (ec_eeprom_waitnotbusyAP(CmdDesc->ecHead.ADP, &estat, EC_TIMEOUTEEP))
	        {
		     if (estat & EC_ESTAT_EMASK) /* error bits are set */
		        {
			     estat = htoes(EC_ECMD_NOP); /* clear error bits */
			     wkc=ec_APWR(CmdDesc->ecHead.ADP, ECT_REG_EEPCTL, sizeof(estat), &estat, EC_TIMEOUTRET);
		        }
		
		    do
			wkc=EcatCmdReq(CmdDesc, &data, slave);
			while ((wkc <= 0) && (cnt++ < retry));
	        }
     
	    }
       //EC_PRINT("%s\n",CmdDesc->cmt);
 	 else 
	    {
		 do
		 {
		   wkc=EcatCmdReq(CmdDesc, &data, slave);
	   
          if (CmdDesc->cnt != ECAT_WCOUNT_DONT_CHECK && CmdDesc->cnt !=wkc)
		   {
		   	 
			       
 				  //EC_PRINT("WRONG WKC\n"); 
 				   wkc=-1; 
			       goto again;
			}			
		    
		 if (((CmdDesc->cnt == ECAT_WCOUNT_DONT_CHECK)||(CmdDesc->cnt ==wkc))&&(CmdDesc->validate==0))
		     {retry=0;}		     
		  if (CmdDesc->validate)
		     {  
		       if(CmdDesc->validateMask)
		        {
			      for (i=0; i<CmdDesc->DataLength; i++)
			        {CmdDesc->data[i]&=CmdDesc->validateDataMask[i];}
				 }
		   
		       for (i=0; i<CmdDesc->DataLength; i++)
		        {
			      if(CmdDesc->data[i]!=CmdDesc->validateData[i])
		          	 {			   
		                
 					//EC_PRINT("VALIDATION FAILED\n"); 
						wkc=-1;
				        goto again;
                      }				   
				  }
				 retry=0;
			    }

			 
again:	if (retry>0)retry--; 
	
		 }
	 while (retry>0);
		}
  
return wkc;
 }
///////////////////////////////////////////////////////////////////////

/*
return the number of init command for this slave and for this transition
*/

uint16 InitCmdNum (uint16 slave, uint16 transition)
{uint16 num=0;
 

	switch (transition){
	  case ECAT_INITCMD_I_P:
	      num=ec_slaveMore[slave].nIPInitCount;
		   break;
      case ECAT_INITCMD_P_I:
	      num=ec_slaveMore[slave].nPIInitCount;
		   break;
	  case ECAT_INITCMD_P_S:
	       num=ec_slaveMore[slave].nPSInitCount;
		   break;
	 case ECAT_INITCMD_S_P:
	       num=ec_slaveMore[slave].nSPInitCount;
		   break;
     case ECAT_INITCMD_S_O:
	      num=ec_slaveMore[slave].nSOInitCount;
		   break;
	 case ECAT_INITCMD_S_I:
	      num=ec_slaveMore[slave].nSIInitCount;
		   break;
     case ECAT_INITCMD_O_S:
	      num=ec_slaveMore[slave].nOSInitCount;
		   break;
     case ECAT_INITCMD_O_P:
	      num=ec_slaveMore[slave].nOPInitCount;
		   break;
	 case ECAT_INITCMD_O_I:
	      num=ec_slaveMore[slave].nOIInitCount;
		   break;
	 case ECAT_INITCMD_I_B:
	     num=ec_slaveMore[slave].nIBInitCount;
		   break;
	 case ECAT_INITCMD_B_I:
	      num=ec_slaveMore[slave].nBIInitCount;
		   break;
	default: 
	      num=0;
	  }	
 return num; 
 }

 ///////////////////////////////////////////////////////////////////////

/*
 brief: send the before slave init cmds for this transition, store in slave_0 (acting as master)
 return 1 if ok, -1 if error
 */

int BeforeSlaveCmd(uint16 transition)
 {
  uint16 NumCmd=0;
  
   NumCmd=InitCmdNum(0,transition);  
  
  if (NumCmd)
 {
  
  InitCmdList *MasterCmdList=ec_slaveMore[0].pSlaveInitCmd; 
  while (NumCmd !=0)
     {
	   
       
	    while ((MasterCmdList->InitCmd.transition&transition)==0x0000)
		      {MasterCmdList=MasterCmdList->nextCmd;}
       
	    if(exec_cmd(&(MasterCmdList->InitCmd),0)<0)
		  {return -1;}
		NumCmd--;
 	    MasterCmdList=MasterCmdList->nextCmd;
     
	  }
	}
 return 1;
  }
 
 ////////////////////////////////////////////////////////////////////////////  

 /*
  brief: send the init command of ALL slaves for this transition, including the mailbox init cmd.
  return 1 if ok, -1 if error 
  */
 
 int SlaveInitCmd (uint16 transition) 
 { uint16 Index, slave, NumCmd,MbxNumCmd;
 uint8 SubIndex;
 int Len,wkc;
 InitCmdList *SlaveCmdList;
   
  InitMboxCmdList *MbxSlaveCmdList;
  for (slave=1; slave<=ec_slavecount; slave++)
    {
	  
      
	  /*slave's init_cmds*/
	  
	  NumCmd=InitCmdNum(slave,transition);  
	if (NumCmd)
	{
 	 SlaveCmdList=ec_slaveMore[slave].pSlaveInitCmd;
	  while (NumCmd!=0)
	      {
		   while ((SlaveCmdList->InitCmd.transition&transition)==0x0000)
		      {SlaveCmdList=SlaveCmdList->nextCmd;}

		   if (exec_cmd(&(SlaveCmdList->InitCmd),slave)<0)
		       {return -1;}
           NumCmd--;
          /*before setting to safe-op, we have to execute the mailbox init cmds*/
		  
		    //the last two init cmds for this transition are 1)set to safe-op; 2)check safe-op
			if ((slave>0)&&(NumCmd==2)&&(transition==ECAT_INITCMD_P_S))   
			    {
				 /*send mbx init cmds*/
	             MbxNumCmd=ec_slaveMore[slave].nPSMbxCount;
	              if (MbxNumCmd)
	               {
				    				  
				    MbxSlaveCmdList=ec_slaveMore[slave].pSlaveMailboxCmd;
		            while (MbxNumCmd!=0)
		                  {
		                   while ((MbxSlaveCmdList->MbInitCmd.transition&ECAT_INITCMD_P_S)==0x0000)
			                    {MbxSlaveCmdList=MbxSlaveCmdList->nextCmd;}
		        	       if (MbxSlaveCmdList->MbInitCmd.protocol==ETHERCAT_MBOX_TYPE_CANOPEN)
			                  {
			                   Index=SWAP(MbxSlaveCmdList->MbInitCmd.Index);
                               SubIndex=MbxSlaveCmdList->MbInitCmd.SubIndex;
						       Len=MbxSlaveCmdList->MbInitCmd.DataLen;
							   wkc=ec_SDOwrite(slave,Index,MbxSlaveCmdList->MbInitCmd.SubIndex,MbxSlaveCmdList->MbInitCmd.CA,Len,MbxSlaveCmdList->MbInitCmd.Data,EC_TIMEOUTRXM);
							   
							   if(wkc==0)
			                      {
						           
								      if ((SubIndex==0)&&(Len==1))
							           {
									    Len++;
										wkc=ec_SDOwrite(slave,Index,SubIndex,FALSE,Len,MbxSlaveCmdList->MbInitCmd.Data,EC_TIMEOUTRXM);
										if(wkc==0) {return -1;}
						       	        }
									  else return -1;
								   }
								}
                          MbxNumCmd--;
			             MbxSlaveCmdList=MbxSlaveCmdList->nextCmd; 
		                 }
	                }
				  
				 }
	        SlaveCmdList=SlaveCmdList->nextCmd;
		    }
	 }
	 
	/*mailbox_init_cmd*/
  if(slave>0)
   {
	if (transition ==ECAT_INITCMD_I_P)
	 {
	    MbxNumCmd=ec_slaveMore[slave].nIPMbxCount;
	              if (MbxNumCmd)
	               {
				    MbxSlaveCmdList=ec_slaveMore[slave].pSlaveMailboxCmd;
		              while (MbxNumCmd!=0)
		                  {
						   while ((MbxSlaveCmdList->MbInitCmd.transition&ECAT_INITCMD_MBX_INIT)==0x0000)
			                      {MbxSlaveCmdList=MbxSlaveCmdList->nextCmd;}
                           if (MbxSlaveCmdList!=NULL)
						    {
		        	          if (MbxSlaveCmdList->MbInitCmd.protocol==ETHERCAT_MBOX_TYPE_CANOPEN)
			                      {
			                       Index=SWAP(MbxSlaveCmdList->MbInitCmd.Index);
                                   SubIndex=MbxSlaveCmdList->MbInitCmd.SubIndex;
						           Len=MbxSlaveCmdList->MbInitCmd.DataLen;
							       wkc=ec_SDOwrite(slave,Index,MbxSlaveCmdList->MbInitCmd.SubIndex,MbxSlaveCmdList->MbInitCmd.CA,Len,MbxSlaveCmdList->MbInitCmd.Data,EC_TIMEOUTRXM);
							   
							   if(wkc==0)
			                      {
						           
								      if ((SubIndex==0)&&(Len==1))
							           {
									    Len++;
										wkc=ec_SDOwrite(slave,Index,SubIndex,FALSE,Len,MbxSlaveCmdList->MbInitCmd.Data,EC_TIMEOUTRXM);
										if(wkc==0) {return -1;}
						       	        }
									  else return -1;
								   }
								}
                            MbxNumCmd--;
			                MbxSlaveCmdList=MbxSlaveCmdList->nextCmd; 
                            }
						  else MbxNumCmd=0;
		                 }
	                }
	  }
	}
   } 
  return 1;
  }
/////////////////////////////////////////////////////////////////////////////

/*
brief: execute the Init-to-PreOp transition.
   send the init cmd for this transition, continue to configure the SlaveList 
   (first part is in the parsing of XML file).
   return 1 if ok, -1 if error
*/
int TransitionIP(void)
{
 uint16 slave, slavec, ssigen,ADPh,topology,estat,topoc;
 uint8 b,h; 
 
 
 /*send before_slave cmds*/
 if(BeforeSlaveCmd(ECAT_INITCMD_I_P)>0)
 {
 
  /*config slaves*/
     
  for (slave = 1; slave <= ec_slavecount; slave++)
        {
		 
		  
          ADPh = (uint16)(1 - slave);
 	    	ec_slave[slave].Itype = etohs(ec_APRDw(ADPh, ECT_REG_PDICTL, EC_TIMEOUTRET));
					
			/* a node offset is used to improve readibility of network frames */
			/* this has no impact on the number of addressable slaves (auto wrap around) */
            ec_APWRw(ADPh, ECT_REG_STADR, htoes(slave + EC_NODEOFFSET) , EC_TIMEOUTRET); /* set node address of slave */
            if (slave == 1) 
				b = 1; /* kill non ecat frames for first slave */
			else 
				b = 0; /* pass all frames for following slaves */
            ec_APWRw(ADPh, ECT_REG_DLCTL, htoes(b), EC_TIMEOUTRET); /* set non ecat frame behaviour */
         } 
	/*slave_init_cmds*/
	
    if (SlaveInitCmd(ECAT_INITCMD_I_P)<0)	
      {return -1;} 

	/*update the ec_slave fields (second part; the first part has been done by XML parsing) this part is taken from ethercatconfig.c*/
  for (slave=1; slave<=ec_slavecount; slave++)
    {
	  
      ec_FPRD(ec_slave[slave].configadr, ECT_REG_ALIAS, sizeof(ec_slave[slave].aliasadr), &(ec_slave[slave].aliasadr), EC_TIMEOUTRET);
	  ec_FPRD(ec_slave[slave].configadr, ECT_REG_EEPSTAT, sizeof(estat), &estat, EC_TIMEOUTRET);
	  estat = etohs(estat);
	  if (estat & EC_ESTAT_R64) /* check if slave can read 8 byte chunks */
 		       plist->ec_slave.eep_8byte = 1;	

	 /* Support DC? */
	 if ((etohs(ec_FPRDw(ec_slave[slave].configadr, ECT_REG_ESCSUP, EC_TIMEOUTSAFE)) & 0x04) > 0)  
                ec_slave[slave].hasdc = TRUE;
     else
                ec_slave[slave].hasdc = FALSE;
	/* extract topology from DL status */		
     topology = etohs(ec_FPRDw(ec_slave[slave].configadr, ECT_REG_DLSTAT, EC_TIMEOUTSAFE)); 
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
		            topology = ec_slave[slave].topology;
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

	/* set default mailbox configuration if slave has mailbox */ //DA ELIMINARE		
	if (plist->ec_slave.mbx_l>0)
			{	
				ec_slave[slave].SMtype[0] = 1;
				ec_slave[slave].SMtype[1] = 2;
				ec_slave[slave].SMtype[2] = 3;
				ec_slave[slave].SMtype[3] = 4;
			
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
					Sec_slave[0].Ebuscurrent += ec_slave[slave].Ebuscurrent;
                }
			ec_eeprom2pdi(slave);
			
	}
  }
   else return -1;
 ec_readstate();
 
  return 1;
 }


 ////////////////////////////////////////////////////////////////////////////


/*
brief: execute all other transitions than IP.
   
   return 1 if ok, -1 if error
*/
int OtherTransitions(uint16 transition)
{
 

 if(BeforeSlaveCmd(transition)>0) 
 {
 
 /*slave_init_cmds*/
	
    if (SlaveInitCmd(transition)<0)	
      {return -1;} 
	
 }
  else 
    return -1;	
  ec_readstate();

 
return 1;

 } 
////////////////////////////////////////////////////////////////////////////

/*
 brief: call the proper function(s) for the requested transition(s), 
 according to the value of Ec_Transition. if the transition is succesfull executed, 
 the value of Ec_Transition is updated.
 return 1 if ok, -1 if error
*/

int MasterStateMachine(void) 
{
 	/*transition_I_P*/
	 if (Ec_Transition&ECAT_INITCMD_I_P)
	    {
		 //if(TransitionIP(pMaster)>0)
 		 if(TransitionIP()>0) 
		     Ec_Transition-=ECAT_INITCMD_I_P;
 
		else
		  
		   return -1; //TODO: REPLACE WITH SOME ERR_CODE
		   
		}   
	/*transition_P_S*/	   
    if (Ec_Transition&ECAT_INITCMD_P_S)
	  {
	    
		if(OtherTransitions(ECAT_INITCMD_P_S)>0)
		     Ec_Transition-=ECAT_INITCMD_P_S;
		else
		   return -1; //TODO: REPLACE WITH SOME ERR_CODE
		}  
	 
	/*transition_S_O*/	   
   if (Ec_Transition&ECAT_INITCMD_S_O)
	    {//if(OtherTransitions(pMaster,ECAT_INITCMD_S_O)>0)
		 if(OtherTransitions(ECAT_INITCMD_S_O)>0)
		     Ec_Transition-=ECAT_INITCMD_S_O;
		   else
		    return -1; //TODO: REPLACE WITH SOME ERR_CODE
         }
	
 /*transition_O_I*/	   
   if (Ec_Transition&ECAT_INITCMD_O_I)
	    {//if(OtherTransitions(pMaster,ECAT_INITCMD_O_I)>0)
		 if(OtherTransitions(ECAT_INITCMD_O_I)>0)
		     Ec_Transition-=ECAT_INITCMD_O_I;
		   else
		    return -1; //TODO: REPLACE WITH SOME ERR_CODE
         }
		 	
 /*transition_O_S*/	   
   if (Ec_Transition&ECAT_INITCMD_O_S)
	    {//if(OtherTransitions(pMaster,ECAT_INITCMD_O_S)>0)
		 if(OtherTransitions(ECAT_INITCMD_O_S)>0)
		     Ec_Transition-=ECAT_INITCMD_O_S;
		   else
		    return -1; //TODO: REPLACE WITH SOME ERR_CODE
         }
		 	
 /*transition_O_P*/	   
   if (Ec_Transition&ECAT_INITCMD_O_P)
	    {//if(OtherTransitions(pMaster,ECAT_INITCMD_O_P)>0)
		 if(OtherTransitions(ECAT_INITCMD_O_P)>0)
		     Ec_Transition-=ECAT_INITCMD_O_P;
		   else
		    return -1; //TODO: REPLACE WITH SOME ERR_CODE
         }
		 	
	/*transition_S_P*/	   
   if (Ec_Transition&ECAT_INITCMD_S_P)
	    {//if(OtherTransitions(pMaster,ECAT_INITCMD_S_P)>0)
		 if(OtherTransitions(ECAT_INITCMD_S_P)>0)
		     Ec_Transition-=ECAT_INITCMD_S_P;
		   else
		    return -1; //TODO: REPLACE WITH SOME ERR_CODE
         }	

 /*transition_S_I*/	   
   if (Ec_Transition&ECAT_INITCMD_S_I)
	    {//if(OtherTransitions(pMaster,ECAT_INITCMD_S_I)>0)
		 if(OtherTransitions(ECAT_INITCMD_S_I)>0)
		     Ec_Transition-=ECAT_INITCMD_S_I;
		   else
		    return -1; //TODO: REPLACE WITH SOME ERR_CODE
         }

 /*transition_P_I*/	   
   if (Ec_Transition&ECAT_INITCMD_P_I)
	    {//if(OtherTransitions(pMaster,ECAT_INITCMD_P_I)>0)
		 if(OtherTransitions(ECAT_INITCMD_P_I)>0)
		     Ec_Transition-=ECAT_INITCMD_P_I;
		   else
		    return -1; //TODO: REPLACE WITH SOME ERR_CODE
         }	
 /*transition_B_I*/	   
   if (Ec_Transition&ECAT_INITCMD_B_I)
	    {//if(OtherTransitions(pMaster,ECAT_INITCMD_B_I)>0)
		 if(OtherTransitions(ECAT_INITCMD_B_I)>0)
		     Ec_Transition-=ECAT_INITCMD_B_I;
		   else
		    return -1; //TODO: REPLACE WITH SOME ERR_CODE
         }
 /*transition_I_B*/	   
   if (Ec_Transition&ECAT_INITCMD_I_B)
	    {//if(OtherTransitions(pMaster,ECAT_INITCMD_I_B)>0)
		 if(OtherTransitions(ECAT_INITCMD_I_B)>0) 
		     Ec_Transition-=ECAT_INITCMD_I_B;
		   else
		    return -1; //TODO: REPLACE WITH SOME ERR_CODE
         }					
	 return 1;	
	   
}

///////////////////////////////////////////////////////////////////////////////
/*
 brief: check if the requested state is different from the current state. in this case
  it calculates and executes the required transition(s).
  return 1 if ok, -1 if error.
 */
int   MasterRequestState(uint16 state)
{
	ec_slaveMore[0].reqState = state;

 if (ec_slaveMore[0].reqState != ec_slave[0].state) 
    {
	 
 	Ec_Transition=GetTransition(); 
      if(Ec_Transition)
	    {
		 
 	    	if (MasterStateMachine()<0) 
		    {
			  //EC_ PRINTF("STATE NO REACHEABLE"); 
			 
             Ec_Transition=0;
			 return -1;
			 }
         }
	   
		
	 }	
   
  return 1;  
}
