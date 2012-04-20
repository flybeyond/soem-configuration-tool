/*******************************************
 * SOEM Configuration tool
 *
 * File    : EcCreateDevice.h
 * Version : 1.3
 * Date    : 19-04-2012
 * History :
 *          1.3, 19-04-2012, complete version for test
 *          1.2, 08-02-2012, deleted SOE, SDO header; changed TEcMailboxCmdDesc
 *          1.1, 24-01-2012, Improved readability
 *          1.0, 21-12-2011, Initial version 
****************************************************************/







#ifndef _EcCreateDevice_H
#define _EcCreateDevice_H
#endif

#include "ethercattype.h"
#include "mxml.h"
#include "ethercatmain.h"

#include <stddef.h>
#include <stdlib.h>


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
#define ECAT_INITCMD_MBX_INIT           0x4000  

#define PHYSICS_Y 0x59
#define PHYSICS_K 0x4B
#define PHYSICS_  0x00

#define PHYSICS_MII  0x03
#define PHYSICS_EBUS  0x02
#define PHYSICS_NOT_IMPL  0x00



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



#define ECAT_WCOUNT_DONT_CHECK  0xffff

#define ENDOF(p)                ((p)+1)

#define SIZEOF_EcInitCmdDesc(p) (sizeof(EcInitCmdDesc) + p->ecHead.dlength + \
    (((p))->validate ? (p->ecHead.dlength) : 0) + \
    (((p))->validateMask ? (p->ecHead.dlength) : 0) + ((p))->cmtLen + 1)


#ifndef SWAPDWORD
#define  SWAPDWORD(d)  ((unsigned long)(((d & 0xff) << 24) | ((d & 0xff00) << 8) | ((d & 0xff0000) >> 8) | ((d & 0xff000000) >> 24)))
#endif

#ifndef SWAPLLONG
#define  SWAPLLONG(ll)      ((ll & 0xff) << 56) | ((ll & 0xff00) << 40) | ((ll & 0xff0000) << 24) | ((ll & 0xff000000) << 8) | ((ll & 0xff00000000) >> 8) | ((ll & 0xff0000000000) >> 24) | ((ll & 0xff000000000000) >> 40) | ((ll & 0xff00000000000000) >> 56)
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
//////////////////////////////////////////////////////////////////////////////
//Mailbox Commands Descriptor
typedef struct TEcMailboxCmdDesc
{
    uint16                   transition;
	uint16                   protocol;
	uint8                    CA;
	char                     cmt[40];
	uint16                   timeout;    // in ms
	uint8                    Ccs;
    uint16                   Index;
    uint8                   SubIndex; // (excl. \0)
    unsigned char           Data[20]; 
    int					    DataLen;
    
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
    
    int                      reqState;
	uint16                      cInitCmds;

	void   *pSlaveInitCmd;  //to be casted to type EcInitCmdDesc defined in NetxEcCreateDevice.h
	void   *pSlaveMailboxCmd; //to be casted to type  EcMailboxCmdDesc defined in NetxEcCreateDevice.h
	uint16  nIPMbxCount;
	uint16  nPSMbxCount;
	uint16  nIPInitCount;
    uint16  nPIInitCount;	
    uint16  nPSInitCount;	
	uint16  nSPInitCount;	
	uint16  nSOInitCount;	
	uint16  nSIInitCount;	
	uint16  nOSInitCount;	
	uint16  nOPInitCount;	
	uint16  nOIInitCount;	
	uint16  nIBInitCount;	
	uint16  nBIInitCount;	
	uint32  configCycTime; //ONLY FOR SLAVE=0 (MASTER)
}  ec_slaveMoret;
	
///////////////////////////////////////////////////////
typedef struct PACKED
{char VarName[EC_MAXNAME + 1];
 char varType[5];
 uint16 VarLength;
 uint16 VarOffset;
 uint8 *VarBase;
 char Slave[EC_MAXNAME + 1];
 } ec_variable;

 typedef struct Tec_VariableList
 {ec_variable variable;
  struct Tec_VariableList *NextVar;
  } ec_VariableList;

/////////////////////////////////////////////////////////

/*************************************
* global variables
**************************************/

extern EcCycDesc Cyclic;
extern ec_slaveMoret ec_slaveMore[EC_MAXSLAVE];
extern ec_VariableList *VariableOutList=NULL, *VariableInList=NULL; 
/**********************************************
* functions prototypes
*
****************************************************/
long text2long(const char *text);
unsigned char text2uchar(const char *text);
unsigned int XmlGetBinDataSize( char *bstrHex );
unsigned char * XmlGetBinData( char * bstrHex, unsigned int *len );
unsigned char *XmlGetBinDataChar( char *bstrHex, unsigned int *lun );
EcInitCmdDesc *ReadECatCmd(mxml_node_t *pCmdNode,mxml_node_t *TopNode);
int CreateMaster(mxml_node_t *pMasterNode, long nSlaves);
int CreateDevice (void); //use this if XML is passed as string buffer
//int CreateDevice (const char *strXMLConfig); //use this if XML is passed as file
 void CreateListInitCmd(InitCmdList **plist);
 void InsertInitCmd(InitCmdList **plist, EcInitCmdDesc *cmd);
void CreateListMboxInitCmd(InitMboxCmdList **plist);
void InsertMboxInitCmd(InitMboxCmdList **plist, EcMailboxCmdDesc *cmd);
 void InsertCycCmd(CycCmdList **plist, EcCmdDesc *cmd);
int CreateSlave(mxml_node_t *pSlave, mxml_node_t *Root, uint16 autoIncrAddr, boolean *bDcEnabled, int slave);
EcMailboxCmdDesc *ReadCANopenCmd(mxml_node_t *pCmdNode,mxml_node_t *TopNode, uint8 CA);
EcMailboxCmdDesc *ReadAoECmd(mxml_node_t *pCmdNode, mxml_node_t *TopNode);
EcMailboxCmdDesc *ReadSoECmd(mxml_node_t *pCmdNode,mxml_node_t *TopNode);
void SetCyclicCmds(mxml_node_t *pCyclic, mxml_node_t *Root, EcCycDesc *pCyclicDesc);
void reset(void);
void SetProcImg(mxml_node_t *ProcImg, mxml_node_t *Root);
 void InsertVariable(ec_VariableList **plist, ec_variable *VarDesc);
 void EraseVarList(ec_VariableList **plist);
 ec_VariableList *findVar(uint16 nVar,ec_VariableList **plist);
 void myreverse(char s[]);
 void Myitoa(int n, char s[]);
 