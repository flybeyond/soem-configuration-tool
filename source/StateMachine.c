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
	  EcatCmdReq(MasterList->InitCmd);//TO BE IMPLEMENTED
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
		 EcatCmdReq(SlaveList->InitCmd);//TO BE IMPLEMENTED
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

/////////////////////////////////////////////////////////////////////////////
boolean TransitionPS(Ecmaster *pMaster)
{
 uint16 slave, slavec, enable, configadr, nNumCmd;
 InitCmdList *SlaveList, *MasterList;
 InitMboxCmdList* SlaveMbxList;
 mxml_node_t  *element, *pNodeSM;
 char *szName;
 /*check if all slave are in PRE_OP*/
  if(ec_statecheck(0, EC_STATE_PRE_OP, EC_TIMEOUTSTATE)!=EC_STATE_PRE_OP)
    return FALSE;
  
 /*send before_slave cmds*/
  nNumCmd=pMaster->nPSInitCmdCount;
  MasterList=pMaster->pIPInitCmd
    while(MasterList&&nNumCmd)
	{
	  EcatCmdReq(MasterList->InitCmd);
	  nNumCmd--;
	  MasterList=MasterList->nextCmd;
	}  
  /*config slaves*/ 
   FILE *fp;
      mxml_node_t *tree;
     
     //fp = fopen(strXMLConfig, "r");
	
	 //create tree
      //	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK); 
	
    
	//tree=CreateXML();
	tree=mxmlLoadString(NULL, data_xmlprova_xml, MXML_OPAQUE_CALLBACK); 
    //fclose(fp);
	
	mxml_node_t *Root = mxmlFindElement(tree, tree,"Config" ,NULL, NULL, MXML_DESCEND);
    if( Root != NULL )
	   mxml_node_t  *pSlaves = mxmlFindElement(Root, tree,"Slave" ,NULL, NULL, MXML_DESCEND);//point to the first <Slave> node
	   
  for (slave=1; slave<=ec_slavecount; slave++)
    {
	 /*slave's init_cmds*/
	 SlaveList=ec_slaveMore[slave].pPSInit;
	  while(SlaveList)
	    {
		 EcatCmdReq(SlaveList->InitCmd); //TO BE IMPLEMENTED
	    SlaveList=SlaveList->nextCmd;
		 }
	 /*Mailbox Init commdands*/
	  SlaveMbxList=ec_slaveMore[slave].pSlaveMailboxCmd;
	  while(SlaveMbxList)
	    {
		 EcatMbxReq(SlaveMbxList->MbInitCmd); //TO BE IMPLEMENTED
	    SlaveMbxList=SlaveMbxList->nextCmd;
		}
	 /*Set Sync Managers*/
	 mxml_node_t  *pNodeProcData = mxmlFindElement(pSlaves, Root,"ProcessData" ,NULL, NULL, MXML_DESCEND);
	  if (pNodeProcData)
	   {
	    mxml_node_t  *pNodeSend = mxmlFindElement(pNodeProcData, pSlaves,"Send" ,NULL, NULL, MXML_DESCEND);
		 if (pNodeSend)
		    element=mxmlFindElement(pNodeSend, pNodeProcData, "BitLength" ,NULL, NULL, MXML_DESCEND);
			ec_slave[slave].Obits=(uint16)(long) text2long(element->child->value.opaque);
			ec_slave[slave].FMMU0func = 1;
			
		mxml_node_t  *pNodeRecv = mxmlFindElement(pNodeProcData, pSlaves,"Recv" ,NULL, NULL, MXML_DESCEND);
		 if (pNodeRecv)
		    element=mxmlFindElement(pNodeRecv, pNodeProcData, "BitLength" ,NULL, NULL, MXML_DESCEND);
			ec_slave[slave].Ibits=(uint16)(long) text2long(element->child->value.opaque);
			ec_slave[slave].FMMU1func = 2;
			
	     if (ec_slave[slave].Ibits > 7)
				ec_slave[slave].Ibytes = (ec_slave[slave].Ibits + 7) / 8;
		 if (ec_slave[slave].Obits > 7)
				ec_slave[slave].Obytes = (ec_slave[slave].Obits + 7) / 8;	
		
		/*set SM0 if present*/
		pNodeSM = mxmlFindElement(pNodeProcData, pSlaves,"Sm0" ,NULL, NULL, MXML_DESCEND);
		if (pNodeSM)
		{ /** SM type 0=unused 1=MbxWr 2=MbxRd 3=Outputs 4=Inputs */
		   element=mxmlFindElement(pNodeSM, pNodeProcData, "Type" ,NULL, NULL, MXML_DESCEND);
		   szName =(char*)(element->child->value.opaque);
		   if (strcmp(szName, "Inputs")==0)
		        {ec_slave[slave].SMtype[0] = 4;
				ec_slave[slave].SM[0].SMlength = htoes((ec_slave[slave].Ibits + 7) / 8);
				ec_slave[slave].FMMU[0].FMMUtype = 2;}
			if (strcmp(szName, "Outputs")==0)
		        {ec_slave[slave].SMtype[0] = 3;	
				element=mxmlFindElement(pNodeSM, pNodeProcData, "DefaultSize" ,NULL, NULL, MXML_DESCEND);
				ec_slave[slave].SM[0].SMlength = htoes((ec_slave[slave].Obits + 7) / 8);
				ec_slave[slave].FMMU[0].FMMUtype = 1;}
			ec_slave[slave].FMMU[0].FMMUactive = 1;	
			element=mxmlFindElement(pNodeSM, pNodeProcData, "StartAddress" ,NULL, NULL, MXML_DESCEND);
		            ec_slave[slave].SM[0].StartAddr = htoes((uint16)(long) text2long(element->child->value.opaque));
			element=mxmlFindElement(pNodeSM, pNodeProcData, "ControlByte" ,NULL, NULL, MXML_DESCEND);
					ec_slave[slave].SM[0].SMflags = htoel((uint32)(long) text2long(element->child->value.opaque));			
		}//end SM0
		
		/*set SM2 if present*/
		pNodeSM = mxmlFindElement(pNodeProcData, pSlaves,"Sm2" ,NULL, NULL, MXML_DESCEND);
		if (pNodeSM)
		{
		    ec_slave[slave].SM[2].SMlength = htoes((ec_slave[slave].Obits + 7) / 8);
			 ec_slave[slave].SMtype[2] = 3;
			 element=mxmlFindElement(pNodeSM, pNodeProcData, "StartAddress" ,NULL, NULL, MXML_DESCEND);
			   ec_slave[slave].SM[2].StartAddr = htoes((uint16)(long) text2long(element->child->value.opaque));
			 element=mxmlFindElement(pNodeSM, pNodeProcData, "ControlByte" ,NULL, NULL, MXML_DESCEND);
				ec_slave[slave].SM[2].SMflags = htoel((uint32)(long) text2long(element->child->value.opaque));
			element=mxmlFindElement(pNodeSM, pNodeProcData, "Enable" ,NULL, NULL, MXML_DESCEND);
		  ec_slave[slave].FMMU[0].FMMUactive ==(uint8)(long) text2long(element->child->value.opaque);
            ec_slave[slave].FMMU[0].FMMUtype = 1;		  
			
		}//end SM2
		
		/*set SM3 if present*/
		pNodeSM = mxmlFindElement(pNodeProcData, pSlaves,"Sm3" ,NULL, NULL, MXML_DESCEND);
		if (pNodeSM)
		{
			 ec_slave[slave].SM[3].SMlength = htoes((ec_slave[slave].Ibits + 7) / 8);
			 ec_slave[slave].SMtype[3] =4;
			 element=mxmlFindElement(pNodeSM, pNodeProcData, "StartAddress" ,NULL, NULL, MXML_DESCEND);
			   ec_slave[slave].SM[3].StartAddr = htoes((uint16)(long) text2long(element->child->value.opaque));
			 element=mxmlFindElement(pNodeSM, pNodeProcData, "ControlByte" ,NULL, NULL, MXML_DESCEND);
				ec_slave[slave].SM[3].SMflags = htoel((uint32)(long) text2long(element->child->value.opaque));
			element=mxmlFindElement(pNodeSM, pNodeProcData, "Enable" ,NULL, NULL, MXML_DESCEND);
		   ec_slave[slave].FMMU[1].FMMUactive=(uint8)(long) text2long(element->child->value.opaque);
		   ec_slave[slave].FMMU[1].FMMUtype = 2;
		} //end SM3
		
	    } //end ProcData
		
	
	
	pSlaves = mxmlFindElement(pSlaves, Root,"Slave" ,NULL, NULL, MXML_DESCEND); //point to the next <Slave> node
	} //end for
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

