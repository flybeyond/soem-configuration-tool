#include "NetxEcCreateDevice.h"

#include "nicdrv.h"
#include "ethercatbase.h"
#include "ethercattype.h"


#define EC_MAXBUF		16

#define NEXT_EcHeader(p)            ((ec_comt *)&(p)[(((ec_comt *)p)->dlength)) + ETYPE_EC_OVERHEAD])

#define MAX_SLAVECMD    50


///////////////////////////////////////////////////////////////////////////////
typedef struct TECAT_SLAVECMD_INFO
{
    uint16      pSlave;
    uint32       invokeId;
    uint16      retry;
	int      RxOffset;
	uint16   wkc; //add 10/01/12
} ECAT_SLAVECMD_INFO;


typedef struct TECAT_SLAVEFRAME_INFO
{
    
    ec_bufT                        *pframe; //pointer to ec_txbuf[idx]
    
	int                         *bufstat;// pointer toec_rxbufstat[idx]
    ECAT_SLAVECMD_INFO          cmdInfo[MAX_SLAVECMD];
    uint32                       nInfo;
    int                         *buflength;// pointer to ec_txbuflength[idx]
   
} ECAT_SLAVEFRAME_INFO;


ECAT_SLAVEFRAME_INFO PendFrame[EC_MAXBUF];


///////////////////////////////////////////////////////////////////////////////



uint16  GetTargetState(uint16 transition);
void   SlaveStartInitCmds(uint16 transition, uint16 nSlave,InitTR * InitCmsLoop);
boolean    SlaveStateMachine(uint16 nSlave, EcMaster *pMaster);
void    SlaveRequestState(uint16 state, uint16 nSlave);
void    MasterStartInitCmds(uint16 stateValue, EC_MASTER_STATE stateNext, EcMaster *pMaster,InitTR *InitCmsLoop);
boolean MasterStateMachine(EcMaster *pMaster);
void    MasterRequestState(uint16 state, EcMaster *pMaster);
ec_err   MasterEcatCmdReq(uint16 nSlave, uint32 invokeId, uint16 wkc, uint8 cmd, uint16 adp, uint16 ado, uint16 len, void *pData);
void InitializeFrame(void);
ec_err EcatCmdFlush(void);
