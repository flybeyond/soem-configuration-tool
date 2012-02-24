/*******************************************
 * SOEM Configuration tool
 *
 * File    : InitCmds.c
 * Version : 1.3
 * Date    : 24-02-2012
 * History :
 *			1.3, 24-02-2012, struct ec_slavet modified
 *          1.2, 24-01-2012, Improved readability
 *			1.1  10-01-2012, add a field in the pending frame to store the expected working counter for a given command (field "Cnt" in ENI XML file)
 *          1.0, 21-12-2011, Initial version 
****************************************************************/


#include "InitCmds.h"


void InitializeFrame(void)
{ int i;
/****inizializzazione PendFrame****************/
 for (i=0; i<EC_MAXBUF; i++)
  {  PendFrame[i].pframe=&ec_txbuf[i];
     PendFrame[i].bufstat=(int *)&ec_txbuf[i];
	   int jk;
	    for (jk=0; jk<MAX_SLAVECMD; jk++)
		   {PendFrame[i].cmdInfo[jk].pSlave=0;
		    PendFrame[i].cmdInfo[jk].invokeId=0;
			PendFrame[i].cmdInfo[jk].retry=0;
			PendFrame[i].cmdInfo[jk].RxOffset=0;
		    }
	  PendFrame[i].nInfo=0;
	  PendFrame[i].buflength=&ec_txbuflength[i];
	  //Pendframe[i].idx=0;
	}
	 
}
///////////////////////////////////////////////////////////////////////////////
uint16  GetTargetState(uint16 transition)
{
    switch ( transition )
    {
    case ECAT_INITCMD_P_I:
    case ECAT_INITCMD_B_I:
    case ECAT_INITCMD_S_I:
    case ECAT_INITCMD_O_I:      return EC_STATE_INIT ;
    case ECAT_INITCMD_I_P:
    case ECAT_INITCMD_S_P:
    case ECAT_INITCMD_O_P:
    case ECAT_INITCMD_MBX_INIT:
                                return EC_STATE_PRE_OP;
    case ECAT_INITCMD_P_S:
    case ECAT_INITCMD_O_S:      return EC_STATE_SAFE_OP;
    case ECAT_INITCMD_S_O:      return EC_STATE_OPERATIONAL;
    case ECAT_INITCMD_I_B:      return EC_STATE_BOOT;
    }
    return 0;
}



//////////////////////////////////////////////////////////////////////////////
///\brief Processes init commands that should be sent in this transition.
void   SlaveStartInitCmds(uint16 transition, uint16 nSlave,InitTR * InitCmsLoop)
{
   uint16 i=0;

    if ( ec_slave[nSlave].cInitCmds == INITCMD_INACTIVE )
        ec_slave[nSlave].cInitCmds = 0;
   
    if ( ec_slave[nSlave].cInitCmds < ec_slave[nSlave].initcmdCnt )
    {//skip InitCmd already sent
      while(i!=ec_slave[nSlave].cInitCmds)  
	         {InitCmsLoop=InitCmsLoop->next;
			  i++;} 
	
	
	//init command found
        EcInitCmdDesc *p    = InitCmsLoop->InitCmd; //m_ppInitCmds[m_cInitCmds];
        //set timeout for init command
        //m_tInitCmds.Start(p->timeout); TODO: to be implemented
        //m_rInitCmds         = p->retries;
        //send init command to slave TODO: to be implemented
        //m_pMaster->EcatCmdReq(this, transition, p->ecHead.cmd, p->ecHead.adp, p->ecHead.ado,
            //(p->ecHead.dlength, p->Data);//SendInitCommand(p, transition);
      ec_slave[nSlave].cInitCmds++;  
    }
    else
    {
        ec_slave[nSlave].cInitCmds=INITCMD_INACTIVE;
        ec_slave[nSlave].currState = GetTargetState(transition);
        
    }
}
///////////////////////////////////////////////////////////////////////////////
///\brief Executes the state machine of the EtherCAT slave.
boolean    SlaveStateMachine(uint16 nSlave, EcMaster *pMaster)
{
    if ( ec_slave[nSlave].currState != ec_slave[nSlave].reqState )
    {
        uint16  transition = 0;
		InitTR *List;
        switch ( ec_slave[nSlave].currState )
        {
        case EC_STATE_INIT:
            switch ( ec_slave[nSlave].reqState )
            {
            case EC_STATE_PRE_OP:
            case EC_STATE_SAFE_OP:
            case EC_STATE_OPERATIONAL:
                transition      = ECAT_INITCMD_I_P;
				List=(InitTR *)&(ec_slave[nSlave].pIPInit);
                break;
            case EC_STATE_BOOT:
                transition      = ECAT_INITCMD_I_B;
            }
            break;
        case EC_STATE_PRE_OP:
            switch ( ec_slave[nSlave].reqState )
            {
            case EC_STATE_INIT:
            case EC_STATE_BOOT:
                transition      = ECAT_INITCMD_P_I;
				List=(InitTR *)&(ec_slave[nSlave].pPIInit);
                break;
            case EC_STATE_SAFE_OP:
            case EC_STATE_OPERATIONAL:
                transition      = ECAT_INITCMD_P_S;
				List=(InitTR *)&(ec_slave[nSlave].pPSInit);
                break;
            }
            break;
        case EC_STATE_BOOT:
            switch ( ec_slave[nSlave].reqState )
            {
            case EC_STATE_INIT:
            case EC_STATE_PRE_OP:
            case EC_STATE_SAFE_OP:
            case EC_STATE_OPERATIONAL:
                transition      = ECAT_INITCMD_B_I;
				List=(InitTR *)&(ec_slave[nSlave].pBIInit);
                break;
            }
        case EC_STATE_SAFE_OP:
            switch ( ec_slave[nSlave].reqState )
            {
            case EC_STATE_INIT:
            case EC_STATE_BOOT:
                transition      = ECAT_INITCMD_S_I;
				List=(InitTR *)&(ec_slave[nSlave].pSIInit);
                break;
            case EC_STATE_PRE_OP:
                transition      = ECAT_INITCMD_S_P;
				List=(InitTR *)&(ec_slave[nSlave].pSPInit);
                break;
            case EC_STATE_OPERATIONAL:
                transition      = ECAT_INITCMD_S_O;
				List=(InitTR *)&(ec_slave[nSlave].pSOInit);
                break;
            }
            break;
        case EC_STATE_OPERATIONAL:
            switch ( ec_slave[nSlave].reqState )
            {
            case EC_STATE_INIT:
            case EC_STATE_BOOT:
                transition      = ECAT_INITCMD_O_I;
				List=(InitTR *)&(ec_slave[nSlave].pOIInit);
                break;
            case EC_STATE_PRE_OP:
                transition      = ECAT_INITCMD_O_P;
				List=(InitTR *)&(ec_slave[nSlave].pOPInit);
                break;
            case EC_STATE_SAFE_OP:
                transition      = ECAT_INITCMD_O_S;
				List=(InitTR *)&(ec_slave[nSlave].pOSInit);
                break;
            }
            break;
        }
        if ( transition && pMaster->m_cInitCmds == INITCMD_INACTIVE )
        {   //transition set and no init commands active -> send first init command defined for this transition
            SlaveStartInitCmds(transition, nSlave, List);
        }
    }
    
    
    ec_slave[nSlave].oldCurrState = ec_slave[nSlave].currState;
    
    return ec_slave[nSlave].reqState == ec_slave[nSlave].currState;
}

///////////////////////////////////////////////////////////////////////////////
///\brief Request EtherCAT slave to change to the specified state.
void    SlaveRequestState(uint16 state, uint16 nSlave)
{
    
    ec_slave[nSlave].reqState = state;
}
////////////////////////////////////////////////////////////////////////////////
///\brief Processes init commands that should be sent in this transition.
void    MasterStartInitCmds(uint16 stateValue, EC_MASTER_STATE stateNext, EcMaster *pMaster, InitTR *InitCmsLoop)
{
    uint16 i=0;
	
   if ( pMaster->m_cInitCmds == INITCMD_INACTIVE)
            pMaster->m_cInitCmds=0;
   
    if ( pMaster->m_cInitCmds < pMaster->initcmdCnt )
	  
    { //skip initcmd already sent
   
      while(i!=pMaster->m_cInitCmds)  
	         {InitCmsLoop=InitCmsLoop->next;
			  i++;}   

    	//init command found
        EcInitCmdDesc *p    =InitCmsLoop->InitCmd; //in InitCmsLoop->InitCmd is stored the ADDRESS of the InitCmd
        //Set timeout for init commands
        //m_tInitCmds.Start(p->timeout); //TODO: to be implemented
        //send init command defined for this transition

       // EcatCmdReq(NULL, stateValue, p->ecHead.cmd, p->ecHead.adp, p->ecHead.ado, p->ecHead.dlength, p->data); //TODO: TO BE IMPLEMENTED/REPLACED with ec_setupdatagram and/or ec_adddatagram
           
       pMaster->m_cInitCmds++;
        
    }
    else
    {
        pMaster->m_cInitCmds = INITCMD_INACTIVE;
        uint16 slaveReq=0;

        //determine which state the master is trying to change to
        //and request this stat from all CEcSlave objects.
        switch ( stateNext )
        {
        case EC_MASTER_STATE_WAIT_SLAVE_P_I:
        case EC_MASTER_STATE_WAIT_SLAVE_S_I:
        case EC_MASTER_STATE_WAIT_SLAVE_O_I:    slaveReq = EC_STATE_INIT; pMaster->startTime = 0; break;
        case EC_MASTER_STATE_WAIT_SLAVE_I_P:
        case EC_MASTER_STATE_WAIT_SLAVE_S_P:
        case EC_MASTER_STATE_WAIT_SLAVE_O_P:    slaveReq = EC_STATE_PRE_OP ; pMaster->startTime = 0; break;
        case EC_MASTER_STATE_WAIT_SLAVE_P_S:
        case EC_MASTER_STATE_WAIT_SLAVE_O_S:    slaveReq = EC_STATE_SAFE_OP ; break;
        case EC_MASTER_STATE_WAIT_SLAVE_S_O:    slaveReq = EC_STATE_OPERATIONAL; break;
        default: break;
        }
        if ( slaveReq )
        {
            for ( i=0; i < ec_slavecount; i++ )
                SlaveRequestState(slaveReq, i);
        }
        //change the current state of the master
        pMaster->m_currState = stateNext;
	}
}
///////////////////////////////////////////////////////////////////////////////
///\brief Executes the state machine of the EtherCAT master.
boolean MasterStateMachine(EcMaster *pMaster)
{
    
    boolean bMasterReady = FALSE;
    boolean bSlavesReady = TRUE;
     uint16 i;
	 InitTR *List;
    //execute state machine of all slaves and
    //check if all slaves are in the requested state (--> bSlaveReady == true )
    for (i=0; i < ec_slavecount; i++ ) 
        {bSlavesReady &= SlaveStateMachine(i, pMaster);}

    uint16              stateValue  = 0;
    EC_MASTER_STATE stateNext   = EC_MASTER_STATE_NONE;
    
    
    

    switch (pMaster->m_currState)
    {
    case EC_MASTER_STATE_INIT:
        //current state
        switch ( pMaster->m_reqState )
        {
        case EC_STATE_INIT :
            //state already in init -> device ready
            bMasterReady = TRUE;
            break;
        case EC_STATE_PRE_OP:
        case EC_STATE_SAFE_OP:
        case EC_STATE_OPERATIONAL:
            //execute the init commands for the transition Init to Pre-Operational, that should
            //be sent before the slave init commands
            stateValue  = ECAT_INITCMD_I_P|ECAT_INITCMD_BEFORE;
			List=(InitTR *)&(pMaster->pIPInitCmd);
            //next state: wait for all slaves to go to Pre-Operational
            stateNext   = EC_MASTER_STATE_WAIT_SLAVE_I_P;
            break;
        }
        break;
    case EC_MASTER_STATE_WAIT_SLAVE_I_P:
        if ( bSlavesReady )
        {   //all slaves are in Pre-Operational
            //-->execute the init commands for the transition Init to Pre-Operational
            stateValue  = ECAT_INITCMD_I_P;
            //next state : Pre-Operational
            stateNext   = EC_MASTER_STATE_PREOP;
        }
        break;

    case EC_MASTER_STATE_PREOP:
        switch ( pMaster->m_reqState )
        {
        case EC_STATE_INIT:
            //execute the init commands for the transition Pre-Operational to Init, that should
            //be sent before the slave init commands
            stateValue  = ECAT_INITCMD_P_I|ECAT_INITCMD_BEFORE;
			List=(InitTR *)&(pMaster->pPIInitCmd);
            //next state: wait for all slaves to go to Init
            stateNext   = EC_MASTER_STATE_WAIT_SLAVE_P_I;
            break;
        case EC_STATE_PRE_OP:
            //state already in Pre-Operational -> device ready
            bMasterReady = TRUE;
            break;
        case EC_STATE_SAFE_OP:
        case EC_STATE_OPERATIONAL:
            //execute the init commands for the transition Pre-Operational to Safe-Operational, that should
            //be sent before the slave init commands
            stateValue  = ECAT_INITCMD_P_S|ECAT_INITCMD_BEFORE;
			List=(InitTR *)&(pMaster->pPSInitCmd);
            //next state: wait for all slaves to go to Safe-Operational
            stateNext   = EC_MASTER_STATE_WAIT_SLAVE_P_S;
            break;
        }
        break;
    case EC_MASTER_STATE_WAIT_SLAVE_P_I:
        if ( bSlavesReady )
        {   //all slaves are in Init
            //-->execute the init commands for the transition Pre-Operational to Init
            stateValue  = ECAT_INITCMD_P_I;
            //next state: Init
            stateNext   = EC_MASTER_STATE_INIT;
        }
        break;
    case EC_MASTER_STATE_WAIT_SLAVE_P_S:
        if ( bSlavesReady )
        {   //all slaves are in Safe-Operational
            //-->execute the init commands for the transition Pre-Operational to Safe-Operational
            stateValue  = ECAT_INITCMD_P_S;
            //next state: Safe-Operational
            stateNext   = EC_MASTER_STATE_SAFEOP;
        }
        break;

    case EC_MASTER_STATE_SAFEOP:
        switch ( pMaster->m_reqState )
        {
        case EC_STATE_INIT:
            stateValue  = ECAT_INITCMD_S_I|ECAT_INITCMD_BEFORE;
			List=(InitTR *)&(pMaster->pSIInitCmd);
            stateNext   = EC_MASTER_STATE_WAIT_SLAVE_S_I;
            break;
        case EC_STATE_PRE_OP:
            stateValue  = ECAT_INITCMD_S_P|ECAT_INITCMD_BEFORE;
			List=(InitTR *)&(pMaster->pSPInitCmd);
            stateNext   = EC_MASTER_STATE_WAIT_SLAVE_S_P;
            break;
        case EC_STATE_SAFE_OP:
            bMasterReady = TRUE;
            break;
        case EC_STATE_OPERATIONAL:
            stateValue  = ECAT_INITCMD_S_O|ECAT_INITCMD_BEFORE;
			List=(InitTR *)&(pMaster->pSOInitCmd);
            stateNext   = EC_MASTER_STATE_WAIT_SLAVE_S_O;
            break;
        }
        break;
    case EC_MASTER_STATE_WAIT_SLAVE_S_I:
        if ( bSlavesReady )
        {
            stateValue  = ECAT_INITCMD_S_I;
            stateNext   = EC_MASTER_STATE_INIT;
        }
        break;
    case EC_MASTER_STATE_WAIT_SLAVE_S_P:
        if ( bSlavesReady )
        {
            stateValue  = ECAT_INITCMD_S_P;
            stateNext   = EC_MASTER_STATE_PREOP;
        }
        break;
    case EC_MASTER_STATE_WAIT_SLAVE_S_O:
        if ( bSlavesReady )
        {
            stateValue  = ECAT_INITCMD_S_O;
            stateNext   = EC_MASTER_STATE_OP;
        }
        break;

    case EC_MASTER_STATE_OP:
        switch ( pMaster->m_reqState )
        {
        case EC_STATE_INIT:
            stateValue  = ECAT_INITCMD_O_I|ECAT_INITCMD_BEFORE;
			List=(InitTR *)&(pMaster->pOIInitCmd);
            stateNext   = EC_MASTER_STATE_WAIT_SLAVE_O_I;
            break;
        case EC_STATE_PRE_OP:
            stateValue  = ECAT_INITCMD_O_P|ECAT_INITCMD_BEFORE;
			List=(InitTR *)&(pMaster->pOPInitCmd);
            stateNext   = EC_MASTER_STATE_WAIT_SLAVE_O_P;
            break;
        case EC_STATE_SAFE_OP:
            stateValue  = ECAT_INITCMD_O_S|ECAT_INITCMD_BEFORE;
			List=(InitTR *)&(pMaster->pOSInitCmd);
            stateNext   = EC_MASTER_STATE_WAIT_SLAVE_O_S;
            break;
        case EC_STATE_OPERATIONAL:
            bMasterReady = TRUE;
            break;

        }
        break;
    case EC_MASTER_STATE_WAIT_SLAVE_O_I:
        if ( bSlavesReady )
        {
            stateValue  = ECAT_INITCMD_O_I;
            stateNext   = EC_MASTER_STATE_INIT;
        }
        break;
    case EC_MASTER_STATE_WAIT_SLAVE_O_P:
        if ( bSlavesReady )
        {
            stateValue  = ECAT_INITCMD_O_P;
            stateNext   = EC_MASTER_STATE_PREOP;
        }
        break;
    case EC_MASTER_STATE_WAIT_SLAVE_O_S:
        if ( bSlavesReady )
        {
            stateValue  = ECAT_INITCMD_O_S;
            stateNext   = EC_MASTER_STATE_SAFEOP;
        }
        break;
        default:
            break;
    }

    if ( stateValue != EC_MASTER_STATE_NONE && pMaster->m_cInitCmds == INITCMD_INACTIVE )
    {  //stateValue specifies a transition in which init commands should be sent
        //and no init command is being processed at the momement.
        pMaster->m_cInitCmds = 0;
		
		
		
        MasterStartInitCmds(stateValue, stateNext, pMaster, List);
    }
    
    
    return (bMasterReady && bSlavesReady) ? TRUE :FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///\brief Request EtherCAT master to change to the specified state.
void    MasterRequestState(uint16 state, EcMaster *pMaster)
{
    
    switch ( state )
    {
    case EC_STATE_INIT:
    case EC_STATE_PRE_OP:
    case EC_STATE_SAFE_OP:
    case EC_STATE_OPERATIONAL:
        pMaster->m_reqState = state;
        pMaster->m_reqStateUser = state;
        break;
    case EC_STATE_RESTORE:
        pMaster->m_reqState = pMaster->m_reqStateUser;
        break;
    }
    
}

/*************************************MA.VI. BEGIN: FUNZIONI PER LA GESTIONE DI FRAME CON PIU' COMANDI*****************************/

///\brief Process EtherCAT command request.


ec_err   MasterEcatCmdReq(uint16 nSlave, uint32 invokeId, uint8 cmd, uint16 adp, uint16 ado, uint16 len, void *pData)
   
{
     boolean more=FALSE;
    //if ( len + ETYPE_EC_OVERHEAD > sizeof(ETHERNET_88A4_MAX_FRAME)-ETHERNET_FRAME_LEN-ETYPE_88A4_IO_HEADER_LEN )
    //    return ECERR_DEVICE_INVALIDSIZE; TODO: TO BE IMPLEMENTED

    int idx=0;
	//check if pending frame exists 
	 while (*(PendFrame[idx].bufstat)!=EC_BUF_ALLOC || idx<EC_MAXBUF) idx++;

    if ( idx < EC_MAXBUF )
    {//pending frame exists, that has not been sent jet
        if ( ( *(PendFrame[idx].buflength) < len + ETYPE_EC_OVERHEAD || PendFrame[idx].nInfo == MAX_SLAVECMD) )
        {   //sub telegram does not fit into pending frame -> send pending frame and create new one
		    /*************Rocco:possibile implementazione*******************/
		     ec_outframe_red(idx); 
             //free(pPendFrame); //una volta mandato il pacchetto, svuoto PendFrame 
			 //NO: ho bisogno di mantenere le informazioni per la ricezione
			/*******************************************************/
			//LONG result = EcatCmdFlush();
           // if ( result != ECERR_NOERR )
             //   return result;
			 
			//create new frame 
			idx=ec_getindex();
		    ec_setupdatagram(PendFrame[idx].pframe,cmd,idx,adp,ado,len, pData);
    	 //TODO: fare qualcosa di simile a ec_push_index()
		}
		else
		  {//add sub telegram to the same frame
		    more= TRUE;
		    PendFrame[idx].cmdInfo[PendFrame[idx].nInfo].RxOffset=ec_adddatagram(PendFrame[idx].pframe, cmd, idx, more, adp, ado, len, pData);
		   }
    }
    
	else
   {   //new pending frame 
	    idx=ec_getindex();
		ec_setupdatagram(PendFrame[idx].pframe,cmd,idx,adp,ado,len, pData);
		//TODO: fare qualcosa di simile a ec_push_index()
		
	}
	//update all the flieds
	
    PendFrame[idx].cmdInfo[PendFrame[idx].nInfo].pSlave     = nSlave;
    PendFrame[idx].cmdInfo[PendFrame[idx].nInfo].invokeId    = invokeId;
    //PendFrame.cmdInfo[PendFrame.nInfo].retry          = nRetry; //TODO: trovare nRetry
    PendFrame[idx].nInfo++;
    
    return EC_ERR_OK;
}
	


////////////////////////////////////////////////////////////////////////////////
/*spedisce i frame pendenti*/
ec_err EcatCmdFlush()
{int idx=0;
	//check if pending frame exists 
	 while (*(PendFrame[idx].bufstat)!=EC_BUF_ALLOC || idx<EC_MAXBUF) idx++;

    if ( idx < EC_MAXBUF )
    {//pending frame exists, that has not been sent jet
       
		     ec_outframe_red(idx); 
		}
	
  return EC_ERR_OK;
  }

	
 
