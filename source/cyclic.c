
#include "cyclic.h"
#include "EcCreateDevice.h"
#include "nicdrv.h"

#define EC_MAXCMD 3
 #define DATAOFFS 10
 #define PROCOFFS 1536 //in the XML file every frame covers 1536 byte of Process Image
 #define PROCIMAGESIZE 4096
 
 uint8 InProc[PROCIMAGESIZE]; //Proces Image Input buffer
 uint8 OutProc[PROCIMAGESIZE]; //Proces Image Input buffer
/*modified version for multicommand frames management*/
typedef struct
{
   uint8   pushed;
   uint8   pulled;
   uint8   idx[EC_MAXBUF];
   uint8   NumCmd[EC_MAXBUF];
   void    *databuffer[EC_MAXBUF][EC_MAXCMD];
   uint16  dataoffset[EC_MAXBUF][EC_MAXCMD];
   uint16  length[EC_MAXBUF][EC_MAXCMD];
   uint16  ExpectedWKC[EC_MAXBUF][EC_MAXCMD];
} ec_MY_idxstackT;

static ec_MY_idxstackT ec_MY_idxstack;



static void ec_MY_pushindex(uint8 NumFrame)
{
  	 uint8 i, NoCmd=CycFrame[NumFrame].nInfo;
 
   if(ec_MY_idxstack.pushed < EC_MAXBUF)
   {ec_MY_idxstack.idx[ec_idxstack.pushed] = CycFrame[NumFrame].nIdx;
    ec_MY_idxstack.NumCmd[ec_idxstack.pushed] =CycFrame[NumFrame].nInfo;
    for (i=0; i<NoCmd; i++)
	 {ec_MY_idxstack.databuffer[ec_idxstack.pushed][i] = CycFrame[NumFrame].RXdatabuffer[i];
	  ec_MY_idxstack.dataoffset[ec_idxstack.pushed][i] = CycFrame[NumFrame].DataOffset[i]; 
      ec_MY_idxstack.length[ec_idxstack.pushed][i] = CycFrame[NumFrame].length[i];
	  ec_MY_idxstack.ExpectedWKC[ec_idxstack.pushed][i] = CycFrame[NumFrame].ExpectedWKC[i];
	 }
      	  
	  ec_MY_idxstack.pushed++; 
	 
     
   }   
}  

static int ec_MY_pullindex(void)
{
	int rval = -1;
	if(ec_MY_idxstack.pulled < ec_MY_idxstack.pushed)
	{
		rval = ec_MY_idxstack.pulled;
		ec_MY_idxstack.pulled++;
	}
	
	return rval;
}
///////////////////////////////////////////////////////////////////////////////////////
 /*return index of cyclic packet*/

 uint8 PrepareCycPacket(uint8 FrameIndex)
 {
    boolean more=TRUE;
    uint8 idx,cmd,i;
   
   uint8 *data;
   uint16 ADP, ADO;
   uint16 Datalength;
   uint16 IOoffset;
   
   CycFrame[FrameIndex].nInfo=ec_Frame[FrameIndex].NumCmd;
   CycFrame[FrameIndex].timeout=EC_TIMEOUTRET;
   //index
	  idx=ec_getindex();
	  CycFrame[FrameIndex].nIdx=idx;
	  
   //buffer
      CycFrame[FrameIndex].FrameBuffer=&ec_txbuf[idx];
	  CycFrame[FrameIndex].FrameStatus=&ec_rxbufstat[idx];
	  
    for (i=0;i<CycFrame[FrameIndex].nInfo;i++)
	    {
		  if (i==(CycFrame[FrameIndex].nInfo-1))//is the last command?
		      more=FALSE;
			  
			//cmd
		 if (ec_Frame[FrameIndex].FrameCmds[i].state&SlaveList->ec_slave.state)
		     cmd=ec_Frame[FrameIndex].FrameCmds[i].cmd;
			 else cmd=EC_CMD_NOP;
			 
		 //length	 
		 Datalength=ec_Frame[FrameIndex].FrameCmds[i].DataLength;
		 CycFrame[FrameIndex].length[i]=Datalength;
		 
		 //data default
		 data=(uint8 *)malloc(Datalength);
		 memset(data, 0, Datalength);
		 
		 //IO map offset
		 IOoffset=ec_Frame[FrameIndex].FrameCmds[i].Offs+DATAOFFS+FrameIndex*PROCOFFS; //(offset of the command in the frame) + (offset of data in the command) + (Number of Frame * 1536)
		 
		 //address & data
		 switch (cmd)
		 { case EC_CMD_LRD:
		      ADP=LO_WORD(ec_Frame[FrameIndex].FrameCmds[i].laddr);
		      ADO=HI_WORD(ec_Frame[FrameIndex].FrameCmds[i].laddr);
			 
			  CycFrame[FrameIndex].RXdatabuffer[i]=&InProc[IOoffset];
		      break;
			  
			case EC_CMD_NOP:
			   ADP=0x0000;
			   ADO=0x0100;
			  break;
			  
			case EC_CMD_LRW:
            case EC_CMD_LWR:
			    ADP=LO_WORD(ec_Frame[FrameIndex].FrameCmds[i].laddr);
		        ADO=HI_WORD(ec_Frame[FrameIndex].FrameCmds[i].laddr);
			    if (ec_slave[0].state==EC_STATE_OPERATIONAL)
			        memcpy(data, &OutProc[IOoffset], Datalength);
				CycFrame[FrameIndex].TXdatabuffer[i]=&OutProc[IOoffset];
                CycFrame[FrameIndex].RXdatabuffer[i]=&InProc[IOoffset];				
				break;
			default:
		    ADP=ec_Frame[FrameIndex].FrameCmds[i].ADP;
			ADO=ec_Frame[FrameIndex].FrameCmds[i].ADO;
		     break;
		 } 
		 //expected wkc
		 CycFrame[FrameIndex].ExpectedWKC[i]=ec_Frame[FrameIndex].FrameCmds[i].cnt;
		 
		 //build frame
		 if (i==0) //the frame is empty?
		      {ec_setupdatagram(&ec_txbuf[idx], cmd, idx, ADP, ADO, Datalength, data);
			  CycFrame[FrameIndex].DataOffset[i]=DATAOFFS+2;} //in ec_rxbuf is stored the frame without Ethernet header (12 byte),
			                                                  //so data of first datagram are located after EtherCAT header(2 byte)
															  //and datagram header (10 byte)
          else
            CycFrame[FrameIndex].DataOffset[i]=ec_adddatagram(&ec_txbuf[idx], cmd, idx, more, ADP, ADO, Datalength, data);
             
    	free(data);	 
		}
	   
	return idx;
}	
///////////////////////////////////////////////////////////////////////////

void SendCycFrame (void)
{
   uint16 i;
   
	
	for (i=0; i<ec_NoFrame;i++)
	{
	  if (i==0)
	    {ec_MY_idxstack.pushed = 0;
	     ec_MY_idxstack.pulled = 0;}
	 /* send frame */
	ec_outframe_red(CycFrame[i].nIdx);     	
	/* push index and Frame Index on stack */
	ec_MY_pushindex(i);
	
	}
	
}
////////////////////////////////////////////////////////////////
int ec_receive_cyclic_packet( int timeout)
{
   int pos, idx;
   int wkc = 0, wkc2, i, WKCoffset, FrameOffset;
    uint8 NumCmd;  
   /* get first index */
   pos = ec_MY_pullindex();
   /* read the same number of frames as send */
   while (pos >= 0)
   {   
      idx = ec_MY_idxstack.idx[pos];
	  NumCmd=ec_MY_idxstack.NumCmd[pos];
	  //receive packet and check last wkc (BRD command)
      wkc2 = ec_waitinframe(idx, timeout);
	  if (wkc2 != ec_MY_idxstack.ExpectedWKC[pos][NumCmd-1])
	        return -1;
			
	  //check other wkc (if any)
	  if (ec_MY_idxstack.NumCmd[pos]>1)
	      for (i=0; i<(NumCmd-1);i++)
		   {
		     WKCoffset=ec_MY_idxstack.dataoffset[pos][i]+ec_MY_idxstack.length[pos][i];
		     wkc=ec_rxbuf[idx][WKCoffset];
			 if (wkc != ec_MY_idxstack.ExpectedWKC[pos][i])
			 return -1;
		   }
      /* check if there is input data in frame */
     
	    FrameOffset=EC_CMDOFFSET;
		for (i=0; i<ec_MY_idxstack.NumCmd[pos];i++)
		{
         if((ec_rxbuf[idx][FrameOffset]==EC_CMD_LRD) || (ec_rxbuf[idx][FrameOffset]==EC_CMD_LRW))
             /* copy input data back to process data buffer */
               memcpy(ec_MY_idxstack.databuffer[pos][i], &ec_rxbuf[idx][ ec_MY_idxstack.dataoffset[pos][i] ], ec_MY_idxstack.length[pos][i]);
		/*find the position of the next command: end of this datagram (dataoffset+datalength+wkc)*/
		FrameOffset=ec_MY_idxstack.dataoffset[pos][i]+ec_MY_idxstack.length[pos][i]+EC_WKCSIZE;
       } 
      
      /* release buffer */
      ec_setbufstat(idx, EC_BUF_EMPTY);
      /* get next index */
      pos = ec_MY_pullindex();
   }   

   return 1;
}   

///////////////////////////////////////////////////////////////////////////
