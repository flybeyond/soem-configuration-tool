/*******************************************
 * SOEM Configuration tool
 *
 * File    : InitCmds.h
 * Version : 1.3
 * Date    : 19-04-2012
 * History :
 *          1.3, 19-04-2012, complete version for test
 *          1.2, 24-01-2012, Improved readability
 *			1.1  10-01-2012, add a field in the pending frame to store the expected working counter for a given command (field "Cnt" in ENI XML file)
 *          1.0, 21-12-2011, Initial version 
****************************************************************/

#ifndef _InitCmds_H
#define _InitCmds_H
#endif

//#include "nicdrv.h"
//#include "ethercatbase.h"

#include "ETH_Register_Include.h"
#include "EcCreateDevice.h"

#define EC_MAXBUF		16

#define NEXT_EcHeader(p)            ((ec_comt *)&(p)[(((ec_comt *)p)->dlength)) + ETYPE_EC_OVERHEAD])

#define MAX_SLAVECMD    50

#define EC_NODEOFFSET		0x1000
#define EC_TEMPNODE			0xffff
/** standard SM0 flags configuration for mailbox slaves */
#define EC_DEFAULTMBXSM0	0x00010026
/** standard SM1 flags configuration for mailbox slaves */
#define EC_DEFAULTMBXSM1	0x00010022
/** standard SM0 flags configuration for digital output slaves */
#define EC_DEFAULTDOSM0		0x00010044




int GetTransition(void);
int MasterStateMachine(void);
int MasterRequestState(uint16 state);
int TransitionIP(void);
int TransitionPS(char *pOMap,char *pImap);
int exec_cmd(EcInitCmdDesc *CmdDesc, uint16 slave);
int EcatCmdReq(EcInitCmdDesc *CmdDesc, uint32 *pData, uint16 Slave);
void set_sm(EcInitCmdDesc *CmdDesc, uint16 slave);
void set_fmmu(EcInitCmdDesc *CmdDesc, uint16 slave);
int OtherTransitions(uint16 transition);
uint16 InitCmdNum (uint16 slave, uint16 transition);
int BeforeSlaveCmd(uint16 transition);
