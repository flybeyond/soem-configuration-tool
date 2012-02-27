/*******************************************
 * SOEM Configuration tool
 *
 * File    : NetxEcCreateDevice.h
 * Version : 1.2
 * Date    : 08-02-2012
 * History :
 *          1.2, 08-02-2012, deleted SOE, SDO header; changed TEcMailboxCmdDesc
 *          1.1, 24-01-2012, Improved readability
 *          1.0, 21-12-2011, Initial version 
****************************************************************/







#ifndef _NetxEcCreateDevice_H
#define _NetxEcCreateDevice_H
#endif

#include "ethercattype.h"
#include "mxml.h"
#include "ethercatmain.h"

#include <stddef.h>


 
#define  HRESULT uint32
#define S_OK            ((HRESULT)0x00000000L)
#define S_FALSE         ((HRESULT)0x00000001L)
#define E_FAIL  ((HRESULT) 0x80004005L)
/******************master Descriptor************************/

#define ECAT_DEVICE_NAMESIZE                31
//addition to enum ec_state
#define EC_STATE_MASK                   0x000F
#define EC_STATE_RESTORE                0x0020


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
#define ECAT_INITCMD_MBX_INIT       0x4000  // after ECAT_INITCMD_I_P to initialize the mailbox


#define ECAT_INITCMD_I_PB           0x0201
#define ECAT_INITCMD_BACKTO_I       0x0524
#define ECAT_INITCMD_BACKTO_P       0x0088
#define ECAT_INITCMD_BEFORE         0x8000




#define INITCMD_INACTIVE    0xffff 

#define PHYSICS_Y 0x59
#define PHYSICS_K 0x4B
#define PHYSICS_  0x00

#define PHYSICS_MII  0x03
#define PHYSICS_EBUS  0x02
#define PHYSICS_NOT_IMPL  0x00

/////////////////////////////////////////////////////////////////////////7
typedef enum EEC_MASTER_STATE
{
    EC_MASTER_STATE_NONE                        = 0x0000,

    EC_MASTER_STATE_INIT                        = 0x0100,

    EC_MASTER_STATE_DCLOCKS,

    EC_MASTER_STATE_WAIT_SLAVE_I_P,

    EC_MASTER_STATE_PREOP                       = 0x0200,

    EC_MASTER_STATE_WAIT_SLAVE_P_I,
    EC_MASTER_STATE_WAIT_SLAVE_P_S,

    EC_MASTER_STATE_SAFEOP                      = 0x0300,

    EC_MASTER_STATE_WAIT_SLAVE_S_I,
    EC_MASTER_STATE_WAIT_SLAVE_S_P,
    EC_MASTER_STATE_WAIT_SLAVE_S_O,

    EC_MASTER_STATE_OP                          = 0x0400,

    EC_MASTER_STATE_WAIT_SLAVE_O_I,
    EC_MASTER_STATE_WAIT_SLAVE_O_P,
    EC_MASTER_STATE_WAIT_SLAVE_O_S,

} EC_MASTER_STATE;

/*************************************************
* TEcMasterDesc
* master descriptor
**************************************************/


typedef struct TEcMasterDesc
{
    uint8                        addr[8];
    //unit16                  macAddr[6];//not needed, because already defined in nicdrv.c
    uint16                  maxSlaves;
    char                       szName[ECAT_DEVICE_NAMESIZE+1];
    uint16                  id;
    struct 
    {
        uint16                       suppressMBoxPolling : 1;
        uint16                       reserved3               : 1;
        uint16                       dcSyncMode              : 4;
        uint16                       ioOpenStateInit     : 1;
        uint16                       logTopologyChanges  : 1;
        uint16                       disableContMeasuring    : 1;
        uint16                       disableContCrcCheck : 1;
        uint16                       dynamicTopology     : 1;
        uint16                       waitForStartIo          : 1;
        uint16                       forceReInit             : 1;
    };

    uint32                      logAddressMBoxStates;   // start address of log. area of sync man states
    uint16                     sizeAddressMBoxStates;  // size of log. area of sync man states
    uint16                      cycOnlyBufferCnt;
    uint16                      initcmdCnt;
    uint16                       initcmdLen;
    uint16                        dcCycleTime;
    uint32                       dcShiftTime;
    uint32                      dcSyncWindow;
    uint32                       reserved2;
	void                        *pMasterInitCmd; //InitCmds list
	void                         *pIPInitCmd; //adresses of InitCmd with transition I_P
	uint16						  nIPInitCmdCount; //number of InitCmd for transition I_P
	void                         *pPIInitCmd; //adresses of InitCmd with transition P_I
 	uint16						  nPIInitCmdCount; //number of InitCmd for transition P_I	
	void                         *pBIInitCmd; //adresses of InitCmd with transition B_I
 	uint16						  nBIInitCmdCount; //number of InitCmd for transition B_I	
	void                         *pSIInitCmd; //adresses of InitCmd with transition S_I
  	uint16						  nSIInitCmdCount; //number of InitCmd for transition S_I	
	void                         *pOIInitCmd; //adresses of InitCmd with transition O_I
  	uint16						  nOIInitCmdCount; //number of InitCmd for transition O_I	
	void                         *pPSInitCmd; //adresses of InitCmd with transition P_S
  	uint16						  nPSInitCmdCount; //number of InitCmd for transition P_S	
	void                         *pSPInitCmd; //adresses of InitCmd with transition S_P
  	uint16						  nSPInitCmdCount; //number of InitCmd for transition S_P	
	void                         *pSOInitCmd; //adresses of InitCmd with transition S_O
  	uint16						  nSOInitCmdCount; //number of InitCmd for transition S_O	
	void                         *pOPInitCmd; //adresses of InitCmd with transition O_P
  	uint16						  nOPInitCmdCount; //number of InitCmd for transition O_P	
	void                         *pOSInitCmd; //adresses of InitCmd with transition O_S
  	uint16						  nOSInitCmdCount; //number of InitCmd for transition O_S	
	uint32                        configCycTime;
 // master state
    EC_MASTER_STATE            m_currState;
    uint16                     m_reqState;
    uint16                     m_reqStateUser;
	
	uint16 m_cInitCmds;//to be initialized as  INITCMD_INACTIVE;
	
	uint64 startTime;	
    
} EcMaster;

///////////////////////////////////////////////////////////////////////////////
typedef struct TEcCycDesc
{
    //ETHERNET_ADDRESS        macTarget;
    uint16                  size;
    uint16                  cntCmd;
    uint16                  syncMapId;
    uint16                  cdlNo;      // 0, 1, 2, 3...   0 = highest prio
    uint16                  imageOffs[2];
    uint16                  imageSize[2];
    
    uint8                   ecatMaster;
    uint8                    state;
    uint16                  reserved2;
	void                   *CycCmds;
} EcCycDesc;


#define ETYPE_EC_OVERHEAD       (EC_HEADERSIZE+EC_WKCSIZE)

#define ETYPE_EC_CMD_LEN(p)  (ETYPE_EC_OVERHEAD+(p->dlength)

#ifndef VG_IN
#define VG_IN       0
#define VG_OUT  1
#endif



typedef enum TECAT_SLAVE_TYPE
{
    ECAT_SLAVE_TYPE_NONE            = 0,
    ECAT_SLAVE_TYPE_SIMPLE          = 1,
    ECAT_SLAVE_TYPE_MAILBOX         = 2,
    ECAT_SLAVE_TYPE_ROUTER          = 3,
    ECAT_SLAVE_TYPE_AX2000B100      = 4,
    ECAT_SLAVE_TYPE_EL67XX          = 5,
} ECAT_SLAVE_TYPE;



#define ECAT_WCOUNT_DONT_CHECK  0xffff

#define ENDOF(p)                ((p)+1)

#define SIZEOF_EcInitCmdDesc(p) (sizeof(EcInitCmdDesc) + p->ecHead.dlength + \
    (((p))->validate ? (p->ecHead.dlength) : 0) + \
    (((p))->validateMask ? (p->ecHead.dlength) : 0) + ((p))->cmtLen + 1)

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
#define ECAT_INITCMD_MBX_INIT       0x4000  // after ECAT_INITCMD_I_P to initialize the mailbox


#define ECAT_INITCMD_I_PB           0x0201
#define ECAT_INITCMD_BACKTO_I       0x0524
#define ECAT_INITCMD_BACKTO_P       0x0088
#define ECAT_INITCMD_BEFORE         0x8000



#ifndef SWAPWORD
#define  SWAPWORD(w)   (HIBYTE((w)) | (LOBYTE((w)) << 8))
#endif
#ifndef SWAPDWORD
//#define  SWAPDWORD(d)  (SWAPWORD(HIWORD(d)) | (SWAPWORD(LOWORD(d)) << 16))
#define  SWAPDWORD(d)  ((unsigned long)(((d & 0xff) << 24) | ((d & 0xff00) << 8) | ((d & 0xff0000) >> 8) | ((d & 0xff000000) >> 24)))
#endif

#ifndef SWAPLLONG
//#define  SWAPLLONG(ll)                            ((ULONGLONG)SWAPDWORD(HIDWORD(ll)) | (((ULONGLONG)SWAPDWORD(LODWORD(ll))) << 32))
//#define  SWAPLLONG(ll)      ((__attribute__((unaligned(8))) ll & 0xff) << 56) | ((__attribute__((unaligned(8))) ll & 0xff00) << 40) | ((ll & 0xff0000) << 24) | ((ll & 0xff000000) << 8) | ((ll & 0xff00000000) >> 8) | ((ll & 0xff0000000000) >> 24) | ((ll & 0xff000000000000) >> 40) | ((ll & 0xff00000000000000) >> 56)
#define  SWAPLLONG(ll)      ((ll & 0xff) << 56) | ((ll & 0xff00) << 40) | ((ll & 0xff0000) << 24) | ((ll & 0xff000000) << 8) | ((ll & 0xff00000000) >> 8) | ((ll & 0xff0000000000) >> 24) | ((ll & 0xff000000000000) >> 40) | ((ll & 0xff00000000000000) >> 56)
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif




/****************************************************
definition of types and commands for mailbox; check if something like these
already exist in SOEM (I've just copied at all from eCos)
****************************************************/

#define ETHERCAT_MBOX_TYPE_ADS              1       // AMS/ADS header follows
#define ETHERCAT_MBOX_TYPE_ETHERNET     2       // ETHERCAT_EOE_HEADER follows
#define ETHERCAT_MBOX_TYPE_CANOPEN          3       // ETHERCAT_CANOPEN_HEADER follows
#define ETHERCAT_MBOX_TYPE_FILEACCESS       4       // ETHERCAT_FOE_HEADER follows
#define ETHERCAT_MBOX_TYPE_SOE              5       // ETHERCAT_SOE_HEADER follows


///////////////////////////////////////////////////////////////////////////////
//changed 27Feb2012
typedef struct TEcMailboxCmdDesc
{
    uint16                   transition;
	uint16                   protocol;
	char                     cmt[40];
	uint16                   timeout;    // in ms
	uint8                    Ccs;
    uint16                   Index;
    uint16                   SubIndex; // (excl. \0)
    uint16                    Data; 
    uint32					 DataLen;
    
} EcMailboxCmdDesc;

#define SIZEOF_EcMailboxCmdDesc(p)  (offsetof(EcMailboxCmdDesc,Data) + p->DataLen + (strlen(p->cmt) + 1))
///////////////////////////////////////////////////////////////////////////////
//Cyclic Commands Descriptors


typedef struct TEcCmdDesc
{
    ec_comt             head;
    uint16              cntSend;
    uint16              cmdSize;
    uint16              imageOffs[2];
    uint16              imageSize[2];
    struct
    {
        uint16             copyInputs  : 1;
        uint16              copyOutputs : 1;
        uint16              mboxState   : 1;
        uint16              dcDeployment    : 1;
    };
    uint16          cntRecv;
    uint8            state;
    uint8            reserved1;
    uint16          reserved2;
	
} EcCmdDesc;




///////////////////////////////////////////////////////////////////////////////
#define ECAT_CYCCMD_OUTPUTS     0x0001
#define ECAT_CYCCMD_INPUTS      0x0002
#define ECAT_CYCCMD_MBOXSTATE   0x0004

typedef struct TEcCycCmdDesc
{
    ec_comt                 ecHead;
    uint16                  cnt;
    uint16                  state;
    uint16                  purpose;
    uint16                  cmtLen; // (excl. \0)
} EcCycCmdDesc;



///////////////////////////////////////////////////////////////////////////////
/*************************************************
* TEcInitCmdDesc
* InitCmds descriptor
**************************************************/
typedef struct TEcInitCmdDesc
{
    ec_comt                      ecHead;
    uint16                       transition;
    uint16                       cnt;
    uint16                       cmtLen; // (excl. \0)
    uint8                       newCycle            : 1;
    uint8                        newFrame            : 1;
    uint8                        validate            : 1;
    uint8                        validateMask    : 1;
    uint8                        invokeId;
    uint16                       timeout;    // in ms
    uint16                       retries;
    uint16                       reserved2;
    uint8                        data[20];
    uint32                       DataLength;	
    uint8                        validateData[20]; // if validate == 1
    uint8                        validateDataMask[20]; // if validateMask == 1
    char                         cmt[40];
} EcInitCmdDesc;

/*********************************************
List: Init Command, Mailbox Init Command, Cyclic Command, Transition
************************************************/
typedef struct PACKED TagInitCmdList
{EcInitCmdDesc InitCmd;
  struct TagInitCmdList *nextCmd;
 } InitCmdList;
 
 typedef struct PACKED TagInitMboxCmdList
 {
  EcMailboxCmdDesc MbInitCmd;
  struct TagInitMboxCmdList *nextCmd;
  }InitMboxCmdList;
  
  typedef struct PACKED TagCycCmdList
 {
  EcCmdDesc CycCmd;
  struct TagCycCmdList *nextCmd;
  }CycCmdList;
  
 //!!!! 
  typedef struct PACKED TagInitTR 
  { EcInitCmdDesc **InitCmd; 
     uint8      before;
	struct TagInitTR *next; 
   }InitTR;

/***************************************
* new fields for ec_slave
****************************************/

typedef struct PACKED Tmore
{
uint16              autoIncAddr;
	#ifndef SLAVE_WITHOUT_SN
     uint32          serialNo;
#endif	 
	
	struct
    {
        uint16                      reserved0               : 1;
        uint16                      mboxOutShortSend        : 1;
        uint16                      cycleMBoxPolling        : 1;
        uint16                      stateMBoxPolling        : 1;
        uint16                      referenceClock          : 1;
        uint16                      forceEndOfBranch        : 1;
        
    };
	
	uint16              initcmdCnt;
    uint16              initcmdLen;
	uint16              mboxCmdCnt;
    uint16              mboxCmdLen;
    uint16             slaveAddressMBoxState;
    uint16             cycleMBoxPollingTime;
    uint16             bootOutStart;
    uint16             bootOutLen;
    uint16             bootInStart;
    uint16             bootInLen;
    uint16             prevPhysAddr;
 //init command
    uint16                      currState;
    uint16                      oldCurrState;
    uint16                      reqState;
	uint16                      cInitCmds;

	void   *pSlaveInitCmd;  //to be casted to type EcInitCmdDesc defined in NetxEcCreateDevice.h
	void   *pSlaveMailboxCmd; //to be casted to type  EcMailboxCmdDesc defined in NetxEcCreateDevice.h
	void   *pIPInit;
	uint16  nIPInitCount;
    void   *pPIInit;
    uint16  nPIInitCount;	
    void   *pPSInit;
    uint16  nPSInitCount;	
	void   *pSPInit;
    uint16  nSPInitCount;	
	void   *pSOInit;
    uint16  nSOInitCount;	
	void   *pSIInit;
    uint16  nSIInitCount;	
	void   *pOSInit;
    uint16  nOSInitCount;	
	void   *pOPInit;
    uint16  nOPInitCount;	
	void   *pOIInit;
    uint16  nOIInitCount;	
	void   *pIBInit;
    uint16  nIBInitCount;	
	void   *pBIInit;
   uint16  nBIInitCount;
}  ec_slaveMoret;	


/*************************************
* global variables
**************************************/
extern EcMaster Master;
extern EcCycDesc Cyclic;
extern ec_slaveMoret ec_slaveMore[EC_MAXSLAVE];

/**********************************************
* functions prototypes
*
****************************************************/
long text2long(const char *text);
unsigned char text2uchar(const char *text);
unsigned int XmlGetBinDataSize( char *bstrHex );
unsigned int XmlGetBinData( char *bstrHex, unsigned char *s, unsigned int nLength );
unsigned char *XmlGetBinDataChar( char *bstrHex, unsigned int *lun );
EcInitCmdDesc *ReadECatCmd(mxml_node_t *pCmdNode,mxml_node_t *TopNode);
int CreateMaster(mxml_node_t *pMasterNode, long nSlaves,  EcMaster *pMaster);
int CreateDevice (void);
 void CreateListInitCmd(InitCmdList **plist);
 void InsertInitCmd(InitCmdList **plist, EcInitCmdDesc *cmd);
void CreateListMboxInitCmd(InitMboxCmdList **plist);
void InsertMboxInitCmd(InitMboxCmdList **plist, EcMailboxCmdDesc *cmd);
 void InsertCycCmd(CycCmdList **plist, EcCmdDesc *cmd);
int CreateSlave(mxml_node_t *pSlave, mxml_node_t *Root, uint16 autoIncrAddr, boolean *bDcEnabled, int slave);
EcInitCmdDesc *ReadECatCmd(mxml_node_t *pCmdNode, mxml_node_t *TopNode);
EcMailboxCmdDesc *ReadAoECmd(mxml_node_t *pCmdNode, mxml_node_t *TopNode);
EcMailboxCmdDesc *ReadSoECmd(mxml_node_t *pCmdNode,mxml_node_t *TopNode);
void SetCyclicCmds(EcMaster *pEcMaster,  mxml_node_t *pCyclic, mxml_node_t *Root, EcCycDesc *pCyclicDesc);
 void InsertInitTR(InitTR **ppInitTRList, EcInitCmdDesc **ppInitCmd, uint8 before);
void reset(void);
