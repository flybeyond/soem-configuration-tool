/*******************************************
 * SOEM Configuration tool
 *
 * File    : NetxEcCreateDevice.h
 * Version : 1.1
 * Date    : 24-01-2012
 * History :
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
	
	//!!!!
	void                         *pIPInitCmd; //adresses of InitCmd with transition I_P
	void                         *pPIInitCmd; //adresses of InitCmd with transition P_I
	void                         *pBIInitCmd; //adresses of InitCmd with transition B_I
	void                         *pSIInitCmd; //adresses of InitCmd with transition S_I
	void                         *pOIInitCmd; //adresses of InitCmd with transition O_I
	void                         *pPSInitCmd; //adresses of InitCmd with transition P_S
	void                         *pSPInitCmd; //adresses of InitCmd with transition S_P
	void                         *pSOInitCmd; //adresses of InitCmd with transition S_O
	void                         *pOPInitCmd; //adresses of InitCmd with transition O_P
	void                         *pOSInitCmd; //adresses of InitCmd with transition O_S
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

#ifdef EC_LITTLE_ENDIAN /*LITTLE_ENDIAN*/
typedef struct TETHERCAT_SDO_HEADER
{
    union
    {
        struct
        {   // Initiate Download Request
            uint8    SizeInd     : 1;
            uint8    Expedited   : 1;
            uint8    Size            : 2;
            uint8    Complete        : 1;
            uint8    Ccs         : 3;    // = 1
        } Idq;
        struct
        {   // Initiate Download Response
            uint8    Reserved        : 5;
            uint8    Scs         : 3;    // = 3
        } Ids;
        struct
        {   // Download Segment Request
            uint8    LastSeg     : 1;
            uint8    Size            : 3;
            uint8    Toggle      : 1;
            uint8    Ccs         : 3;    // = 0
        } Dsq;
        struct
        {   // Download Segment Response
            uint8    Reserved        : 4;
            uint8    Toggle      : 1;
            uint8    Scs         : 3;    // = 1
        } Dss;
        struct
        {   // Initiate Upload Request
            uint8    Reserved        : 4;
            uint8    Complete        : 1;
            uint8    Ccs         : 3;    // = 2
        } Iuq;
        struct
        {   // Initiate Upload Response
            uint8    SizeInd     : 1;
            uint8    Expedited   : 1;
            uint8    Size            : 2;
            uint8    Reserved        : 1;
            uint8    Scs         : 3;    // = 2
        } Ius;
        struct
        {   // Upload Segment Request
            uint8    Reserved        : 4;
            uint8    Toggle      : 1;
            uint8    Ccs         : 3;    // = 3
        } Usq;
        struct
        {   // Upload Segment Response
            uint8    LastSeg     : 1;
            uint8    Size            : 3;
            uint8    Toggle      : 1;
            uint8    Scs         : 3;    // = 0
        } Uss;
        struct
        {   // Abort Transfer
            uint8    Reserved        : 5;
            uint8    Ccs         : 3;    // = 4
        } Abt;
        uint8        CS;                             //
    };
    uint16  Index;                          //
    uint8       SubIndex;                       //
    uint32       Data;                               //
} ETHERCAT_SDO_HEADER;
#else /*BIG_ENDIAN*/
typedef struct TETHERCAT_SDO_HEADER
{
    union
    {
        struct
        {   // Initiate Download Request
            uint8    Ccs         : 3;    // = 1
            uint8    Complete        : 1;
            uint8    Size            : 2;
            uint8    Expedited   : 1;
            uint8    SizeInd     : 1;
        } Idq;
        struct
        {   // Initiate Download Response
            uint8    Scs         : 3;    // = 3
            uint8    Reserved        : 5;
        } Ids;
        struct
        {   // Download Segment Request
            uint8    Ccs         : 3;    // = 0
            uint8    Toggle      : 1;
            uint8    Size            : 3;
            uint8    LastSeg     : 1;
        } Dsq;
        struct
        {   // Download Segment Response
            uint8    Scs         : 3;    // = 1
            uint8    Toggle      : 1;
            uint8    Reserved        : 4;
        } Dss;
        struct
        {   // Initiate Upload Request
            uint8    Ccs         : 3;    // = 2
            uint8    Complete        : 1;
            uint8    Reserved        : 4;
        } Iuq;
        struct
        {   // Initiate Upload Response
            uint8    Scs         : 3;    // = 2
            uint8    Reserved        : 1;
            uint8    Size            : 2;
            uint8    Expedited   : 1;
            uint8    SizeInd     : 1;
        } Ius;
        struct
        {   // Upload Segment Request
            uint8    Ccs         : 3;    // = 3
            uint8    Toggle      : 1;
            uint8    Reserved        : 4;
        } Usq;
        struct
        {   // Upload Segment Response
            uint8    Scs         : 3;    // = 0
            uint8    Toggle      : 1;
            uint8    Size            : 3;
            uint8    LastSeg     : 1;
        } Uss;
        struct
        {   // Abort Transfer
            uint8    Ccs         : 3;    // = 4
            uint8    Reserved        : 5;
        } Abt;
        uint8        CS;                             //
    };
    uint16  Index_swapj;                            //
    uint8        SubIndex;                       //
    uint32       Data_swapdj;                                //
} ETHERCAT_SDO_HEADER;
#endif
#ifdef EC_LITTLE_ENDIAN /*LITTLE_ENDIAN*/
typedef struct TETHERCAT_SOE_HEADER
{
    uint8    OpCode      : 3;    // 0 = unused, 1 = readReq, 2 = readRes, 3 = writeReq, 4 = writeRes
                                    // 5 = notification (command changed notification)
    uint8    InComplete  : 1;    // more follows
    uint8    Error           : 1;    // an error word follows
    uint8    DriveNo     : 3;    // drive number

    union
    {
        struct
        {
            uint8        DataState   : 1;
            uint8        Name        : 1;
            uint8        Attribute   : 1;
            uint8        Unit        : 1;
            uint8        Min         : 1;
            uint8        Max         : 1;
            uint8        Value       : 1;
            uint8        Default     : 1;
        };
        uint8    Elements;
    };

    union
    {
        uint16  IDN;            // SoE IDN                  if (InComplete==0)
        uint16  FragmentsLeft;  // Pending fragments            if (InComplete==1)
    };
//  union
//  {
//      BYTE                                Data[]      // rest of mailbox data     if (Error==0 && OpCode!=ECAT_SOE_OPCODE_EMGCY)
//      USHORT                          ErrorCode   //                                  if (Error==1)
//      ETHERCAT_EMERGENCY_HEADER   Emergency   // rest of mailbox data     if (Error==0 && OpCode==ECAT_SOE_OPCODE_EMGCY)
//  };
} ETHERCAT_SOE_HEADER;
#else /*BIG_ENDIAN*/
typedef struct TETHERCAT_SOE_HEADER
{
    uint8    DriveNo     : 3;    // drive number
    uint8    Error           : 1;    // an error word follows
    uint8    InComplete  : 1;    // more follows
    uint8    OpCode      : 3;    // 0 = unused, 1 = readReq, 2 = readRes, 3 = writeReq, 4 = writeRes
                                    // 5 = notification (command changed notification)

    union
    {
        struct
        {
            uint8        Default     : 1;
            uint8        Value           : 1;
            uint8        Max         : 1;
            uint8       Min         : 1;
            uint8        Unit            : 1;
            uint8        Attribute   : 1;
            uint8        Name            : 1;
            uint8        DataState   : 1;
        };
        uint8    Elements;
    };

    union
    {
        uint16  IDN_swapj   ;           // SoE IDN                  if (InComplete==0)
        uint16  FragmentsLeft_swapj;    // Pending fragments            if (InComplete==1)
    };
//  union
//  {
//      BYTE                                Data[]      // rest of mailbox data     if (Error==0 && OpCode!=ECAT_SOE_OPCODE_EMGCY)
//      USHORT                          ErrorCode   //                                  if (Error==1)
//      ETHERCAT_EMERGENCY_HEADER   Emergency   // rest of mailbox data     if (Error==0 && OpCode==ECAT_SOE_OPCODE_EMGCY)
//  };
} ETHERCAT_SOE_HEADER;
#endif

///////////////////////////////////////////////////////////////////////////////
typedef struct TEcMailboxCmdDesc
{
    uint16                   transition;
    uint16                   protocol;
    uint32                    dataLen;
    uint16                   cmtLen; // (excl. \0)
    uint16                   timeout;    // in ms
    uint16                   retries;
    uint16                   reserved1;
    uint32                    reserved2[4];
    union
    {
        struct
        {
            ETHERCAT_SDO_HEADER sdo;
            uint8                        data[20];        
        } coe;
        struct
        {
            ETHERCAT_SOE_HEADER head;
            uint32                        attribute;
            uint8                        data[20];        
        } soe;
        
        struct
        {
            uint16                   nameSize;
            uint16                   reserved;
            uint32                       password;
            char                        name[1];        // no \0
            uint8                        data[20];
        } foe;
        uint8                        data[20];            
    };
  char    cmt[40];
} EcMailboxCmdDesc;

#define SIZEOF_EcMailboxCmdDesc(p)  (offsetof(EcMailboxCmdDesc, data) + p->dataLen + p->cmtLen + 1)
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
int CreateDevice (EcMaster *pMaster, EcCycDesc *Cyclic);
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

