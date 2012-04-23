/*******************************************
 * SOEM Configuration tool
 *
 * File    : EcCreateDevice.c
 * Version : 1.3
 * Date    : 19-04-2012
 * History :
 *          1.3, 19-04-2012, complete version for test
 *          1.2, 08-02-2012, temporarily disabled SoE and AoE; changed parsing <CoE>,<InitCmds> parsing
 *          1.1, 24-01-2012, Improved readability
 *          1.0, 21-12-2011, Initial version 
****************************************************************/

#include "EcCreateDevice.h"
#include "xmlprova.c"
#include "ethercatmain.h"
#include "ethercatbase.h"

EcCycDesc Cyclic;


// define if debug printf is needed
//#define EC_DEBUG

#ifdef EC_DEBUG
#define EC_PRINT printf
#else
#define EC_PRINT(...) do {} while (0)
#endif

 //////////////////////////////////////////////////////
 void InsertVariable(ec_VariableList **plist, ec_variable *VarDesc)
 {ec_VariableList *temp, *tail;

  temp=(ec_VariableList *)malloc(sizeof(ec_VariableList));
  unsigned char *pByte=(unsigned char *)&(temp->variable);
  memcpy(pByte, VarDesc, sizeof(ec_variable));
  
  temp->NextVar=NULL;
  if (*plist == NULL)
   { *plist=temp;
     }  
   else
    { tail=*plist;
	  while (tail->NextVar != NULL)
	    {tail=tail->NextVar;}
		tail->NextVar=temp;
	}
  }
void EraseVarList(ec_VariableList **plist)
{ec_VariableList *temp, *tail;
  ec_variable *SlaveDesc; 
  
 if (*plist != NULL)
 {
  temp=*plist;
  tail=temp->NextVar;
    do
	{
	 SlaveDesc=&(temp->variable);
	 free(SlaveDesc);
	 temp=tail;
	 tail=tail->NextVar;
	 }
	 while (tail != NULL);
  }	  
 }	

 

ec_VariableList *findVar(uint16 nVar,ec_VariableList **plist)
 {   ec_VariableList *temp;
	uint16 i=0;
	temp=*plist;
    while (i<nVar)
	{temp=temp->NextVar;
	 i++;}
	 return temp;
}

void myreverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

void Myitoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     myreverse(s);
 }
////////////////////////////////////////////////
void reset(void)
{
 
   
   memset(&Cyclic,0, sizeof(EcCycDesc)); 
   EraseSlaveList(&SlaveList); 
   EraseVarList(&VariableOutList);
   EraseVarList(&VariableInList);
    
 }

/////////////////////////////////////////////////////////////////////////////
long text2long(const char *text)
{
  long res = 0;
  int  i;

  for (i=0; text[i] >= '0' && text[i] <= '9'; i++)
    res = 10*res + (text[i] - '0');
  return res;
}

unsigned char text2uchar(const char *text)
{
  unsigned char res = 0;
  int  i;

  for (i=0; text[i] >= '0' && text[i] <= '9'; i++)
    res = 10*res + (text[i] - '0');
  return res;
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
unsigned int XmlGetBinDataSize( char * bstrHex )
{
    return ( strlen(bstrHex)/2 );
}

/////////////////////////////////////////////////////////////////////////////
unsigned char * XmlGetBinData( char * bstrHex, unsigned int *len )
{
    unsigned int i;
    *len = XmlGetBinDataSize( bstrHex );
    unsigned char * s = (unsigned char *) malloc(len);
    for ( i = 0; i < len; i++ )
    {
        char tmp = bstrHex[2*i+1];
        if ( tmp >= '0' && tmp <= '9' )
            s[i]  = (unsigned char)(tmp - '0');
        else if ( tmp >= 'a' && tmp <= 'f' )
            s[i]  = 10 + (unsigned char)(tmp - 'a');
        else if ( tmp >= 'A' && tmp <= 'F' )
            s[i]  = 10 + (unsigned char)(tmp - 'A');
        else
            break;
        tmp = bstrHex[2*i];
        if ( tmp >= '0' && tmp <= '9' )
            s[i]  |= (unsigned char)(tmp - '0') << 4;
        else if ( tmp >= 'a' && tmp <= 'f' )
            s[i]  |= (10 + (unsigned char)(tmp - 'a')) << 4;
        else if ( tmp >= 'A' && tmp <= 'F' )
            s[i]  |= (10 + (unsigned char)(tmp - 'A')) << 4;
        else
            break;
    }
    if ( i != len )
    {
printf("XmlGetBinData : Errore bstrHex %s\n", bstrHex);
        free(s);
        len = 0;
        return NULL;
    }
    else
        return s;
}

/////////////////////////////////////////////////////////////////////////////
unsigned char *XmlGetBinDataChar( char *bstrHex, unsigned int *lun )
{
    unsigned int i,len;
    len = XmlGetBinDataSize( bstrHex );
	*lun=len;
    unsigned char *s = (unsigned char *) malloc(len);
    for ( i = 0; i < len; i++ )
    {
        char tmp = bstrHex[2*i+1];
        if ( tmp >= '0' && tmp <= '9' )
            s[i]  = (unsigned char)(tmp - '0');
        else if ( tmp >= 'a' && tmp <= 'f' )
            s[i]  = 10 + (unsigned char)(tmp - 'a');
        else if ( tmp >= 'A' && tmp <= 'F' )
            s[i]  = 10 + (unsigned char)(tmp - 'A');
        else
            break;
        tmp = bstrHex[2*i];
        if ( tmp >= '0' && tmp <= '9' )
            s[i]  |= (unsigned char)(tmp - '0') << 4;
        else if ( tmp >= 'a' && tmp <= 'f' )
            s[i]  |= (10 + (unsigned char)(tmp - 'a')) << 4;
        else if ( tmp >= 'A' && tmp <= 'F' )
            s[i]  |= (10 + (unsigned char)(tmp - 'A')) << 4;
        else
            break;
    }
    if ( i != len )
    {
printf("XmlGetBinData : Errore bstrHex %s\n", bstrHex);
        free(s);
        len = 0;
        return NULL;
    }
    else
        return s;
}


/************************************************************
\brief Operations on list
***************************************************************/
void CreateListInitCmd(InitCmdList **plist)
 { *plist=NULL;}
 
void CreateListMboxInitCmd(InitMboxCmdList **plist)
 { *plist=NULL;} 
 
 
void InsertInitCmd(InitCmdList **plist, EcInitCmdDesc *cmd)
{ InitCmdList *temp; 
  InitCmdList *tail;
  temp=(InitCmdList *)malloc(sizeof(InitCmdList));
  unsigned char *pByte=(unsigned char *)&(temp->InitCmd);
  memcpy(pByte, cmd, sizeof(EcInitCmdDesc));
  
  temp->nextCmd=NULL;
  if (*plist == NULL)
   { *plist=temp;
     }  
   else
    { tail=*plist;
	  while (tail->nextCmd != NULL)
	    {tail=tail->nextCmd;}
		tail->nextCmd=temp;
	}
 } 
 
 void InsertMboxInitCmd(InitMboxCmdList **plist, EcMailboxCmdDesc *cmd)
{ InitMboxCmdList *temp, *tail;
  temp=(InitMboxCmdList *)malloc(sizeof(InitMboxCmdList));
  
  unsigned char *pByte=(unsigned char *)&(temp->MbInitCmd);
  memcpy(pByte, cmd, sizeof(EcMailboxCmdDesc));
  temp->nextCmd=NULL;
  if (*plist == NULL)
   { *plist=temp;}  
   else
    { tail=*plist;
	  while (tail->nextCmd != NULL)
	    {tail=tail->nextCmd;}
		tail->nextCmd=temp;
	}
 } 
 
 void InsertCycCmd(CycCmdList **plist, EcCmdDesc *cmd)
 {CycCmdList *temp, *tail;
  temp=(CycCmdList *)malloc(sizeof(CycCmdList));
  
  unsigned char *pByte=(unsigned char *)&(temp->CycCmd);
  memcpy(pByte, cmd, sizeof(EcCmdDesc));
  temp->nextCmd=NULL;
  if (*plist == NULL)
   { *plist=temp;}  
   else
    { tail=*plist;
	  while (tail->nextCmd != NULL)
	    {tail=tail->nextCmd;}
		tail->nextCmd=temp;
	}
  }
  
 
/////////////////////////////////////////////////////////////////////////////

/************************************************************
Read the <InitCmds> nodes in the ENI XML file and store the information in the proper list
@param[IN] pCmdNode = pointer to the current node in the ENI XML file
@param[IN] TopNode = pointer to the upper node in the ENI XML file
@return pDesc = command descriptor that stores the information about the read command
***************************************************************/
EcInitCmdDesc *ReadECatCmd(mxml_node_t *pCmdNode, mxml_node_t *TopNode)
{
    EcInitCmdDesc *pDesc = NULL;
    unsigned char *pData = NULL;
    unsigned char *pVal = NULL;
    unsigned char  *pMask = NULL;
    uint32 hr = EC_ERR_OK;
    int i;	
	

    char *strComment = NULL;
    char *strCommentTemp = NULL;


    
        uint32 nData;
        uint16 nTimeout=0;
      
        mxml_node_t *spValidate, *spNode;
        spNode=mxmlFindElement(pCmdNode, TopNode, "Comment", NULL, NULL, MXML_DESCEND_FIRST);
        if( spNode != NULL ) {
            strCommentTemp = (char *)spNode->child->value.opaque;
            if (strncmp(strCommentTemp, "![CDATA[", 8)==0) {
                strCommentTemp = strCommentTemp+8;
                if (strCommentTemp[strlen(strCommentTemp)-2] == ']')
                    strCommentTemp[strlen(strCommentTemp)-2] = '\0';
            }
            strComment = (char *) malloc(strlen(strCommentTemp)+1);
            strcpy(strComment, strCommentTemp);
        }
        spNode=mxmlFindElement(pCmdNode, TopNode, "Data", NULL, NULL, MXML_DESCEND_FIRST);
        if( spNode != NULL )
        {
            pData = XmlGetBinDataChar((char *)spNode->child->value.opaque, &nData);
            
 			
        }
        else
        {
		   spNode=mxmlFindElement(pCmdNode, TopNode, "DataLength", NULL, NULL, MXML_DESCEND_FIRST);
            nData = (uint64)(long)text2long(spNode->child->value.opaque);
        }

        spValidate = mxmlFindElement(pCmdNode, TopNode, "Validate", NULL, NULL, MXML_DESCEND_FIRST);
        if( spValidate != NULL )
        {   //Reads out how data that has been read from the slave should be validated
            uint32 nVal=0;
            spNode = mxmlFindElement(spValidate, TopNode, "Data", NULL, NULL, MXML_DESCEND_FIRST);
            if (spNode != NULL)
			{pVal = XmlGetBinDataChar((char *)spNode->child->value.opaque, &nVal);}
            if( nVal != nData )
            {
                EC_PRINT("Unable to read node\n");

                hr = EC_ERR_NOK; goto throwj2;

            }
            

            spNode = mxmlFindElement(spValidate, pCmdNode, "DataMask", NULL, NULL, MXML_DESCEND_FIRST);
            if( spNode != NULL )
            {
                pMask = XmlGetBinDataChar((char*)spNode->child->value.opaque, &nVal);
                if( nVal != nData )
                {
                    EC_PRINT("Unable to read node\n");

                    hr = EC_ERR_NOK; goto throwj2;

                }
                
            }

            spNode = mxmlFindElement(spValidate, pCmdNode, "Timeout", NULL, NULL, MXML_DESCEND_FIRST);
            if( spNode != NULL )
            {
                nTimeout = (unsigned short)(long)text2long(spNode->child->value.opaque);
            }
        }

        
        pDesc = malloc(sizeof(EcInitCmdDesc));

        memset(pDesc, 0, sizeof(EcInitCmdDesc));
        pDesc->timeout = nTimeout;
        pDesc->cmtLen = strlen(strComment);
		pDesc->ecHead.dlength=nData;
		
        
        //Read transitions during which this command should be sent
        mxml_node_t *spTransitions = mxmlFindElement(pCmdNode, TopNode, "Transition", NULL, NULL, MXML_DESCEND_FIRST);
        pDesc->transition = 0;
 	    mxml_node_t *element;	
        for( element = spTransitions; element; element = mxmlFindElement(element, pCmdNode, "Transition", NULL, NULL, MXML_DESCEND))
        {
            if( strcmp(element->child->value.opaque, "IP") == 0 )
                pDesc->transition |= ECAT_INITCMD_I_P;
 			 else if( strcmp(element->child->value.opaque, "II") == 0 )
                pDesc->transition |= ECAT_INITCMD_I_P;	
            else if( strcmp(element->child->value.opaque, "PS") == 0 )
                pDesc->transition |= ECAT_INITCMD_P_S;
            else if( strcmp(element->child->value.opaque, "PI") == 0 )
                pDesc->transition |= ECAT_INITCMD_P_I;
            else if( strcmp(element->child->value.opaque, "SP") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_P;
            else if( strcmp(element->child->value.opaque, "SO") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_O;
            else if( strcmp(element->child->value.opaque, "SI") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_I;
            else if( strcmp(element->child->value.opaque, "OS") == 0  )
                pDesc->transition |= ECAT_INITCMD_O_S;
            else if( strcmp(element->child->value.opaque, "OP") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_P;
            else if( strcmp(element->child->value.opaque, "OI") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_I;
 		    else if( strcmp(element->child->value.opaque, "BI") == 0 )
                pDesc->transition |= ECAT_INITCMD_B_I;
			else if( strcmp(element->child->value.opaque, "IB") == 0 )
                pDesc->transition |= ECAT_INITCMD_I_B;		
        }

        spNode = mxmlFindElement(pCmdNode, TopNode, "BeforeSlave", NULL, NULL, MXML_DESCEND_FIRST);
        if( spNode )
        {   //specifies if the command should be executed before the slave commands
            if( (long) text2long(spNode->child->value.opaque) == 1 )
                pDesc->transition |= ECAT_INITCMD_BEFORE;
        }

        spNode = mxmlFindElement(pCmdNode, TopNode, "Requires", NULL, NULL, MXML_DESCEND_FIRST);
        if( spNode != NULL )
        {
            if( strcmp(spNode->child->value.opaque, "cycle") == 0 )
            {
                pDesc->newCycle = 1;
            }
            else if( strcmp(spNode->child->value.opaque, "frame") == 0 )
            {
                pDesc->newFrame  = 1;
            }
        }

        //read command type
		spNode = mxmlFindElement(pCmdNode, TopNode, "Cmd", NULL, NULL, MXML_DESCEND_FIRST);
	if (spNode != NULL)
	{
		
        pDesc->ecHead.command = (unsigned char) text2uchar(spNode->child->value.opaque);
        switch (pDesc->ecHead.command)
        {
        case EC_CMD_LRD:
        case EC_CMD_LWR:
        case EC_CMD_LRW:
            
			spNode = mxmlFindElement(pCmdNode, TopNode, "Addr", NULL, NULL, MXML_DESCEND_FIRST);
			
            pDesc->ecHead.laddr = SWAPDWORD((unsigned long)(long) text2long(spNode->child->value.opaque)); 
			
            break;
        default:
            //read address page and offset
			spNode = mxmlFindElement(pCmdNode, TopNode, "Adp", NULL, NULL, MXML_DESCEND_FIRST);
			
            pDesc->ecHead.ADP = (unsigned short)(long) text2long(spNode->child->value.opaque);
			spNode = mxmlFindElement(pCmdNode, TopNode, "Ado", NULL, NULL, MXML_DESCEND_FIRST);
			
            pDesc->ecHead.ADO = (unsigned short)(long) text2long(spNode->child->value.opaque);
        }
	}

        spNode = mxmlFindElement(pCmdNode, TopNode, "Cnt", NULL, NULL, MXML_DESCEND_FIRST);
        if( spNode != NULL )
        {   //read working counter
            pDesc->cnt = (unsigned short)(long) text2long(spNode->child->value.opaque);
        }
        else
        {
            pDesc->cnt = ECAT_WCOUNT_DONT_CHECK; 
        }
        spNode = mxmlFindElement(pCmdNode, TopNode, "Retries", NULL, NULL, MXML_DESCEND_FIRST);
        pDesc->retries = (unsigned short)(long) text2long(spNode->child->value.opaque);

        if ( pData )
            
		 { 
		   for (i=0;i<nData;i++)
			   {pDesc->data[i]=(unsigned char *)*(pData+i);}
		 }
		 
 	
        
        if ( pVal && pMask )
        {
            pDesc->validate     = TRUE;
            pDesc->validateMask = TRUE;
            for (i=0;i<nData;i++)
			   {pDesc->validateData[i]=(unsigned char *)*(pVal+i);}
            for ( i=0;i<nData;i++)
			   {pDesc->validateDataMask[i]=(unsigned char *)*(pMask+i);}
            
        }
        else if ( pVal )
        {
            pDesc->validate     = TRUE;
            for (i=0;i<nData;i++)
			   {pDesc->validateData[i]=(unsigned char *)*(pVal+i);}
           
        }
      
        strcpy(pDesc->cmt,strComment);
 	     pDesc->DataLength=(uint32)nData;	
        
        if ( strComment )
            free(strComment);
       
    

throwj2:
    if (hr == EC_ERR_NOK)
    {
        free(pDesc);
        pDesc = NULL;
        if ( strComment )
            free(strComment);
    }


    if( pData )
        free(pData);
    if( pVal )
        free(pVal);
    if( pMask )
        free(pMask);
    return pDesc;
}
///////////////////////////////////////////////////////////////////////////////

/************************************************************
Read the <CoE>,<InitCmds> nodes in the ENI XML file and store the information in the proper list
@param[IN] pCmdNode = pointer to the current node in the ENI XML file
@param[IN] TopNode = pointer to the upper node in the ENI XML file
@return pDesc = command descriptor that stores the information about the read command

History:
       ver. 1.3, 27-02-2012, EcMailboxCmdDesc has been modified
        ver. 1.2, 08-02-2012, EcMailboxCmdDesc has been changed
		ver  1.1, 24-01-2012 Initial version

***************************************************************/

EcMailboxCmdDesc *ReadCANopenCmd(mxml_node_t *pCmdNode,mxml_node_t *TopNode, uint8 CA)
{
    EcMailboxCmdDesc *pDesc = NULL;
    unsigned char * pData = NULL;
    mxml_node_t *element, *spNode;
    char *strComment = NULL;
    char *strCommentTemp = NULL;
    int i;
        //int nData = 0;
		pDesc = (EcMailboxCmdDesc *) malloc(sizeof(EcMailboxCmdDesc));
        memset(pDesc, 0, sizeof(EcMailboxCmdDesc));

 	spNode=mxmlFindElement(pCmdNode, TopNode, "Comment", NULL, NULL, MXML_DESCEND_FIRST);
        if( spNode != NULL ) {
            strCommentTemp = (char *)spNode->child->value.opaque;
            if (strncmp(strCommentTemp, "![CDATA[", 8)==0) {
                strCommentTemp = strCommentTemp+8;
                if (strCommentTemp[strlen(strCommentTemp)-2] == ']')
                    strCommentTemp[strlen(strCommentTemp)-2] = '\0';
            }
            strComment = (char *) malloc(strlen(strCommentTemp)+1);
            strcpy(strComment, strCommentTemp);
        }
		strcpy(pDesc->cmt,strComment);
		pDesc->protocol         = ETHERCAT_MBOX_TYPE_CANOPEN;	
        spNode =mxmlFindElement(pCmdNode, TopNode, "Data", NULL, NULL, MXML_DESCEND_FIRST);
         
        if( spNode != NULL )
        {
            //Read data that should be sent
            //pDesc->Data = (unsigned short)(unsigned short)(long)text2long(spNode->child->value.opaque);
 		    pData = XmlGetBinDataChar((char *)spNode->child->value.opaque, &(pDesc->DataLen));	
        }

        
        
        //pDesc->DataLen          = nData;
        	
        if ( pData )
            
		 { 
		   for (i=0;i<pDesc->DataLen ;i++)
			   {pDesc->Data[i]=(unsigned char *)*(pData+i);}
		 }

        //Read transitions during which this command should be sent
        mxml_node_t *spTransitions = mxmlFindElement(pCmdNode, TopNode, "Transition", NULL, NULL, MXML_DESCEND_FIRST);

        pDesc->transition = 0;
        for( element = spTransitions; element; element = mxmlFindElement(element, pCmdNode, "Transition", NULL, NULL, MXML_DESCEND) )
        {
            if( strcmp(element->child->value.opaque, "IP") == 0 )
                pDesc->transition |= ECAT_INITCMD_MBX_INIT;
            else if( strcmp(element->child->value.opaque, "PS") == 0 )
                pDesc->transition |= ECAT_INITCMD_P_S;
            else if( strcmp(element->child->value.opaque, "PI") == 0 )
                pDesc->transition |= ECAT_INITCMD_P_I;
            else if( strcmp(element->child->value.opaque, "SP") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_P;
            else if( strcmp(element->child->value.opaque, "SO") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_O;
            else if( strcmp(element->child->value.opaque, "SI") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_I;
            else if( strcmp(element->child->value.opaque, "OS") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_S;
            else if( strcmp(element->child->value.opaque, "OP") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_P;
            else if( strcmp(element->child->value.opaque, "OI") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_I;
        }

        if( (spNode = mxmlFindElement(pCmdNode, TopNode, "Timeout", NULL, NULL, MXML_DESCEND_FIRST) ))
            pDesc->timeout = (unsigned short)(long) text2long(spNode->child->value.opaque);
        //SDO index
        if( (spNode = mxmlFindElement(pCmdNode, TopNode, "Index", NULL, NULL, MXML_DESCEND_FIRST) ))
            pDesc->Index = SWAP((unsigned short)(long) text2long(spNode->child->value.opaque));
        //SDO Subindex
        if( (spNode = mxmlFindElement(pCmdNode, TopNode, "SubIndex", NULL, NULL, MXML_DESCEND_FIRST)) )
            pDesc->SubIndex = (unsigned char) text2uchar(spNode->child->value.opaque);
        //command type 1 = SDO initiate upload ; 2 = SDO initiate download
        if( (spNode = mxmlFindElement(pCmdNode, TopNode, "Ccs", NULL, NULL, MXML_DESCEND_FIRST)) )
            pDesc->Ccs = (unsigned char) text2uchar(spNode->child->value.opaque);
        //Complete Access 
        pDesc->CA=CA;    
    if( pData )
        free(pData);
 	if ( strComment )
        free(strComment);	
    return pDesc;
}

///////////////////////////////////////////////////////////////////////////////
/************************************************************
Read the <SoE>,<InitCmds> nodes in the ENI XML file and store the information in the proper list
@param[IN] pCmdNode = pointer to the current node in the ENI XML file
@param[IN] TopNode = pointer to the upper node in the ENI XML file
@return pDesc = command descriptor that stores the information about the read command

History:
       ver 1.2, 08-02-2012, temporarily disabled
       ver 1.1, 24-01-2012, Initial version
***************************************************************/
/*
EcMailboxCmdDesc *ReadSoECmd(mxml_node_t *pCmdNode,mxml_node_t *TopNode)
{
    EcMailboxCmdDesc *pDesc = NULL;
    unsigned char * pData = NULL;
    mxml_node_t *element, *spNode;
    char *strComment = NULL;
    char *strCommentTemp = NULL;
    
        uint32 nData = 0;
 	spNode=mxmlFindElement(pCmdNode, TopNode, "Comment", NULL, NULL, MXML_DESCEND_FIRST);
        if( spNode != NULL ) {
            strCommentTemp = (char *)spNode->child->value.opaque;
            if (strncmp(strCommentTemp, "![CDATA[", 8)==0) {
                strCommentTemp = strCommentTemp+8;
                if (strCommentTemp[strlen(strCommentTemp)-2] == ']')
                    strCommentTemp[strlen(strCommentTemp)-2] = '\0';
            }
            strComment = (char *) malloc(strlen(strCommentTemp)+1);
            strcpy(strComment, strCommentTemp);
        }
			
        spNode =mxmlFindElement(pCmdNode, TopNode, "Data", NULL, NULL, MXML_DESCEND_FIRST);
         
        if( spNode != NULL )
        {
            //Read data that should be sent
            pData = XmlGetBinDataChar((char *) spNode->child->value.opaque, &nData);
        }
        pDesc = (EcMailboxCmdDesc *) malloc(sizeof(EcMailboxCmdDesc));
        memset(pDesc, 0, sizeof(EcMailboxCmdDesc));
        pDesc->protocol         = ETHERCAT_MBOX_TYPE_SOE;
        pDesc->dataLen              = sizeof(pDesc->soe.head)+ sizeof(pDesc->soe.attribute)+ nData;
        pDesc->cmtLen               = 0;
        pDesc->coe.sdo.Data      = SWAPDWORD(nData);
		strcpy(pDesc->cmt,strComment);
        if (pData)
            memcpy(&pDesc->soe.data, pData, nData);

        //Read transitions during which this command should be sent
        mxml_node_t *spTransitions = mxmlFindElement(pCmdNode, TopNode, "Transition", NULL, NULL, MXML_DESCEND_FIRST);

        pDesc->transition = 0;
        for( element = spTransitions; element; element = mxmlFindElement(element, pCmdNode, "Transition", NULL, NULL, MXML_DESCEND) )
        {
            if( strcmp(element->child->value.opaque, "IP") == 0 )
                pDesc->transition |= ECAT_INITCMD_MBX_INIT;
            else if( strcmp(element->child->value.opaque, "PS") == 0 )
                pDesc->transition |= ECAT_INITCMD_P_S;
            else if( strcmp(element->child->value.opaque, "PI") == 0 )
                pDesc->transition |= ECAT_INITCMD_P_I;
            else if( strcmp(element->child->value.opaque, "SP") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_P;
            else if( strcmp(element->child->value.opaque, "SO") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_O;
            else if( strcmp(element->child->value.opaque, "SI") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_I;
            else if( strcmp(element->child->value.opaque, "OS") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_S;
            else if( strcmp(element->child->value.opaque, "OP") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_P;
            else if( strcmp(element->child->value.opaque, "OI") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_I;
        }

        if( (spNode = mxmlFindElement(pCmdNode, TopNode, "Timeout", NULL, NULL, MXML_DESCEND_FIRST) ))
            pDesc->timeout = (unsigned short)(long) text2long(spNode->child->value.opaque);

        //SoE OpCode
        if( (spNode = mxmlFindElement(pCmdNode, TopNode, "OpCode", NULL, NULL, MXML_DESCEND_FIRST)) )
            pDesc->soe.head.OpCode = (unsigned char) text2uchar(spNode->child->value.opaque);
        //SoE DriveNo
        if( (spNode = mxmlFindElement(pCmdNode, TopNode, "DriveNo", NULL, NULL, MXML_DESCEND_FIRST)) )
            pDesc->soe.head.DriveNo = (unsigned char) text2uchar(spNode->child->value.opaque);
        //SoE IDN
        if( (spNode = mxmlFindElement(pCmdNode, TopNode, "IDN", NULL, NULL, MXML_DESCEND_FIRST)) )
            pDesc->soe.head.IDN = (unsigned char) text2uchar(spNode->child->value.opaque);
        //Read Value
        pDesc->soe.head.Value = 1;


    if( pData )
        free(pData);
	if ( strComment )
        free(strComment);	
    return pDesc;
}
*/
///////////////////////////////////////////////////////////////////////////////
/************************************************************
Read the <AoE>,<InitCmds> nodes in the ENI XML file and store the information in the proper list
@param[IN] pCmdNode = pointer to the current node in the ENI XML file
@param[IN] TopNode = pointer to the upper node in the ENI XML file
@return pDesc = command descriptor that stores the information about the read command

History:
       ver 1.2, 08-02-2012, temporarily disabled
       ver 1.1, 24-01-2012, Initial version
***************************************************************/
/* 
EcMailboxCmdDesc *ReadAoECmd(mxml_node_t *pCmdNode, mxml_node_t *TopNode)
{
    EcMailboxCmdDesc *pDesc = NULL;
    unsigned char * pData = NULL;
    mxml_node_t *element, *spNode;
    char *strComment = NULL;
    char *strCommentTemp = NULL;
    
        uint32 nData = 0;
 	spNode=mxmlFindElement(pCmdNode, TopNode, "Comment", NULL, NULL, MXML_DESCEND_FIRST);
        if( spNode != NULL ) {
            strCommentTemp = (char *)spNode->child->value.opaque;
            if (strncmp(strCommentTemp, "![CDATA[", 8)==0) {
                strCommentTemp = strCommentTemp+8;
                if (strCommentTemp[strlen(strCommentTemp)-2] == ']')
                    strCommentTemp[strlen(strCommentTemp)-2] = '\0';
            }
            strComment = (char *) malloc(strlen(strCommentTemp)+1);
            strcpy(strComment, strCommentTemp);
        }
			
        spNode =mxmlFindElement(pCmdNode, TopNode, "Data", NULL, NULL, MXML_DESCEND_FIRST);
        
        if( spNode != NULL )
        {
            //Read data that should be sent
            pData = XmlGetBinDataChar((char *) spNode->child->value.opaque, &nData);
        }

        pDesc = (EcMailboxCmdDesc *) malloc(sizeof(EcMailboxCmdDesc));
        memset(pDesc, 0, sizeof(EcMailboxCmdDesc));
        pDesc->protocol         = ETHERCAT_MBOX_TYPE_ADS;
        pDesc->dataLen              = nData;
        pDesc->cmtLen               = 0;
		strcpy(pDesc->cmt,strComment);
        if ( pData )
            memcpy(&pDesc->data, pData, nData);

        //Read transitions during which this command should be sent
        mxml_node_t *spTransitions = mxmlFindElement(pCmdNode, TopNode, "Transition", NULL, NULL, MXML_DESCEND_FIRST);

        pDesc->transition = 0;
        for( element = spTransitions; element; element = mxmlFindElement(element, pCmdNode, "Transition", NULL, NULL, MXML_DESCEND) )
        {
            if( strcmp(element->child->value.opaque, "IP") == 0 )
                pDesc->transition |= ECAT_INITCMD_MBX_INIT;
            else if( strcmp(element->child->value.opaque, "PS") == 0 )
                pDesc->transition |= ECAT_INITCMD_P_S;
            else if( strcmp(element->child->value.opaque, "PI") == 0 )
                pDesc->transition |= ECAT_INITCMD_P_I;
            else if( strcmp(element->child->value.opaque, "SP") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_P;
            else if( strcmp(element->child->value.opaque, "SO") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_O;
            else if( strcmp(element->child->value.opaque, "SI") == 0 )
                pDesc->transition |= ECAT_INITCMD_S_I;
            else if( strcmp(element->child->value.opaque, "OS") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_S;
            else if( strcmp(element->child->value.opaque, "OP") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_P;
            else if( strcmp(element->child->value.opaque, "OI") == 0 )
                pDesc->transition |= ECAT_INITCMD_O_I;
        }

       if( (spNode = mxmlFindElement(pCmdNode, TopNode, "Timeout", NULL, NULL, MXML_DESCEND_FIRST) ))
            pDesc->timeout = (unsigned short)(long) text2long(spNode->child->value.opaque);

    if( pData )
        free(pData);
	if ( strComment )
        free(strComment);	
	
    return pDesc;
}
*/
///////////////////////////////////////////////////////////////////////////////

/************************************************************
Read the <Master> node in the ENI XML file and store the information in a global variable
@param[IN] pMasterNode = pointer to the <Master>  node in the ENI XML file
@param[IN] nSlaves = total number of slaves
information are stored in ec_slave[0] and ec_slaveMore[0]

@return 1 if successful
***************************************************************/

int CreateMaster(mxml_node_t *pMasterNode, long nSlaves)
{
    
    char * szName;
    mxml_node_t *node;
	
   
	node=mxmlFindElement(pMasterNode, pMasterNode,"Info" ,NULL, NULL,MXML_DESCEND);
	node=mxmlFindElement(node, pMasterNode,"Name" ,NULL, NULL,MXML_DESCEND);
    szName = (char *)node->child->value.opaque;
    if (strncmp(szName, "![CDATA[", 8)==0) {
        szName = szName+8;
        if (szName[strlen(szName)-2] == ']')
                    szName[strlen(szName)-2] = '\0';
    }
   
    strncpy(ec_slave[0].name, szName, ECAT_DEVICE_NAMESIZE);
    ec_slave[0].name[ECAT_DEVICE_NAMESIZE] = 0;
   
   
    //Read master init commands
    mxml_node_t *spCmds= mxmlFindElement(pMasterNode, pMasterNode, "InitCmds", NULL, NULL, MXML_DESCEND);
	InitCmdList *pMasterCmdList;
	
    if( spCmds != NULL )
	
    { 
	  CreateListInitCmd(&pMasterCmdList);
      mxml_node_t *spCmd= mxmlFindElement(spCmds, pMasterNode, "InitCmd", NULL, NULL, MXML_DESCEND);
	  if( spCmd != NULL )
      {
		mxml_node_t *element;  
         for(element = spCmd; element != NULL; element=mxmlFindElement(element, spCmds,"InitCmd" ,NULL, NULL,MXML_NO_DESCEND))
          {
            EcInitCmdDesc *pCmdDesc = ReadECatCmd(element,pMasterNode); 
            if( pCmdDesc )
             {
                ec_slaveMore[0].initcmdCnt++;
                //ec_slaveMore[0].initcmdLen += SIZEOF_EcInitCmdDesc(pCmdDesc); 
 		    	InsertInitCmd(&pMasterCmdList, pCmdDesc);	
                free(pCmdDesc);
             }
         }
      }
      
	
  
   ec_slaveMore[0].pSlaveInitCmd=(InitCmdList *)pMasterCmdList;
  
     InitCmdList *loop=pMasterCmdList;
   while (loop != NULL)
   { if( (loop->InitCmd.transition & ECAT_INITCMD_I_P))
         { ec_slaveMore[0].nIPInitCount++;}
	  if( (loop->InitCmd.transition & ECAT_INITCMD_P_I))
         {ec_slaveMore[0].nPIInitCount++;}
	if( (loop->InitCmd.transition & ECAT_INITCMD_B_I))
         { ec_slaveMore[0].nBIInitCount++;}
	if( (loop->InitCmd.transition & ECAT_INITCMD_S_I))
         { ec_slaveMore[0].nSIInitCount++;}
	if( (loop->InitCmd.transition & ECAT_INITCMD_O_I))
         {ec_slaveMore[0].nOIInitCount++;}
	if( (loop->InitCmd.transition & ECAT_INITCMD_P_S))
         {ec_slaveMore[0].nPSInitCount++;}
	if( (loop->InitCmd.transition & ECAT_INITCMD_S_P))
         {ec_slaveMore[0].nSPInitCount++;}
	if( (loop->InitCmd.transition & ECAT_INITCMD_S_O))
         {ec_slaveMore[0].nSOInitCount++;}
	if( (loop->InitCmd.transition & ECAT_INITCMD_O_P))
         {ec_slaveMore[0].nOPInitCount++;}
	if( (loop->InitCmd.transition & ECAT_INITCMD_O_S))
         {ec_slaveMore[0].nOSInitCount++;}
	loop=loop->nextCmd;
    }
	EC_PRINT("Number of before_slave Init Commands for transition:\n");
    EC_PRINT("Transition I_P: %d\n",ec_slaveMore[0].nIPInitCount);
    EC_PRINT("Transition P_I: %d\n",ec_slaveMore[0].nPIInitCount);
    EC_PRINT("Transition B_I: %d\n",ec_slaveMore[0].nBIInitCount);
    EC_PRINT("Transition S_I: %d\n",ec_slaveMore[0].nSIInitCount);
    EC_PRINT("Transition O_I: %d\n",ec_slaveMore[0].nOIInitCount);
    EC_PRINT("Transition P_S: %d\n",ec_slaveMore[0].nPSInitCount);
    EC_PRINT("Transition S_P: %d\n",ec_slaveMore[0].nSPInitCount);
    EC_PRINT("Transition S_O: %d\n",ec_slaveMore[0].nSOInitCount);
    EC_PRINT("Transition O_S: %d\n",ec_slaveMore[0].nOSInitCount);
    EC_PRINT("Transition O_P: %d\n",ec_slaveMore[0].nOPInitCount);
    EC_PRINT("Transition I_B: %d\n",ec_slaveMore[0].nIBInitCount);
	return 1;
   }
   else 
   {
    EC_PRINT("Unable to read init command from node Master\n\r");
    return -1;
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/************************************************************
Read the <Slave> node in the ENI XML file and store the information in ec_slave
@param[IN] pSlave = pointer to the <Slave>  node in the ENI XML file
@param[IN] Root = pointer to the Root of the ENI XML file
@param[IN] autoIncrAddr = auto incremental address of the current slave
@param[IN] bDcEnabled = TRUE if Distributed clock is enable for the current slave
@param[IN] slave = index of the current slave
@param[out] ec_slave[slave] and ec_slaveMore[slave] store the information read from the ENI XML file

@return 1 if successful
History:
       ver 1.2, 08-02-2012, temporarily disabled parsing SoeE and AoE
       ver 1.1, 24-01-2012, Initial version
***************************************************************/

int CreateSlave(mxml_node_t *pSlave, mxml_node_t *Root, uint16 autoIncrAddr, boolean *bDcEnabled, int slave)
{
    char *szName;
	mxml_node_t *element;
    mxml_node_t *spNode;
	int Sizephy;	
	
	
    
	 ec_slaveMore[slave].autoIncAddr = autoIncrAddr;
    
    mxml_node_t *spNodeInfo = mxmlFindElement(pSlave, Root, "Info", NULL, NULL, MXML_DESCEND);
	mxml_node_t *spNodePhysAddr = mxmlFindElement(spNodeInfo, pSlave, "PhysAddr", NULL, NULL, MXML_DESCEND);
	
    if( spNodePhysAddr == NULL )
	  {
        EC_PRINT("PhysAddr not present\n\r");
        return -1;
      }
   
    ec_slave[slave].configadr=etohs((uint16)(long)text2long(spNodePhysAddr->child->value.opaque)); 
   
    mxml_node_t *spNodeVendorId = mxmlFindElement(spNodeInfo, pSlave, "VendorId", NULL, NULL, MXML_DESCEND);
	
    if( spNodeVendorId == NULL )
	{
        EC_PRINT("VendorId not present\n\r");
        return -1;
    }
    ec_slave[slave].eep_id = (uint32)(long) text2long(spNodeVendorId->child->value.opaque);	
   
    mxml_node_t *spNodeProductCode = mxmlFindElement(spNodeInfo, pSlave, "ProductCode", NULL, NULL, MXML_DESCEND);
	
    if( spNodeProductCode == NULL )
	{
        EC_PRINT("ProductCode not present\n\r");
        return -1;
    }
    ec_slave[slave].eep_man = (uint32)(long) text2long(spNodeProductCode->child->value.opaque);	
    
    mxml_node_t *spNodeRevisionNo =mxmlFindElement(spNodeInfo, pSlave, "RevisionNo", NULL, NULL, MXML_DESCEND);

    if( spNodeRevisionNo == NULL )
	{
        EC_PRINT("RevisionNo not present\n\r");
        return -1;
    }
    ec_slave[slave].eep_rev = (uint32)(long) text2long(spNodeRevisionNo->child->value.opaque);	
   
#ifndef SLAVE_WITHOUT_SN
    mxml_node_t *spNodeSerialNo =mxmlFindElement(spNodeInfo, pSlave, "SerialNo", NULL, NULL, MXML_DESCEND);
 
    if( spNodeSerialNo == NULL )
	{
        EC_PRINT("SerialNo not present\n\r");
        return -1;
    }
    ec_slaveMore[slave].serialNo = (uint32)(long) text2long(spNodeSerialNo->child->value.opaque);	
    
#endif


    mxml_node_t *pszName = mxmlFindElement(spNodeInfo, pSlave, "Name", NULL, NULL, MXML_DESCEND_FIRST);
	    szName =(char*)(pszName->child->value.opaque);
	
    if (strncmp(szName, "![CDATA[", 8)==0) {
        szName = szName+8;
        if (szName[strlen(szName)-2] == ']')
                    szName[strlen(szName)-2] = '\0';
    }
     strncpy(ec_slave[slave].name, szName, EC_MAXNAME);
	 ec_slave[slave].name[EC_MAXNAME] = 0;
    EC_PRINT("Terminal: %s\n",ec_slave[slave].name);
  mxml_node_t *pszPhysics = mxmlFindElement(spNodeInfo, pSlave, "Physics", NULL, NULL, MXML_DESCEND_FIRST);
  if (pszPhysics)
    {
	  szName =(char*)(pszPhysics->child->value.opaque);
	  Sizephy=strlen(szName);
	  int i;
	  for (i=0;i<Sizephy;i++)
	    { 
		  if (szName[i]==PHYSICS_Y)
		     
 			 ec_slave[slave].ptype|=(PHYSICS_MII<<2*i); 
		  if (szName[i]==PHYSICS_K)
		      
 			 ec_slave[slave].ptype|=(PHYSICS_EBUS<<2*i); 
		  if (szName[i]==PHYSICS_)
		     
 			   ec_slave[slave].ptype|=(PHYSICS_NOT_IMPL<<2*i); 
		  }
	   }
	    

mxml_node_t *spMailbox=mxmlFindElement(pSlave, Root, "Mailbox", NULL, NULL, MXML_DESCEND_FIRST);
    
    if( spMailbox != NULL)
    {   EC_PRINT("Mailbox configuration:\n");
	  mxml_node_t *spSend=mxmlFindElement(spMailbox, pSlave, "Send", NULL, NULL, MXML_DESCEND_FIRST);
	
        //Read size and address of the output mailbox
		if (spSend != NULL)
        {  EC_PRINT("Output Mailbox: ");
		  spNode=mxmlFindElement(spSend, spMailbox, "Start", NULL, NULL, MXML_DESCEND);
            if (spNode != NULL)
                {ec_slave[slave].mbx_wo = (unsigned short)(long) text2long(spNode->child->value.opaque);
				 ec_slave[slave].SMtype[0] = 1;
				EC_PRINT("Start %4x", ec_slave[slave].mbx_wo);
				}	
				spNode=mxmlFindElement(spSend, spMailbox, "Length", NULL, NULL, MXML_DESCEND);
            if (spNode != NULL)
                {
				 ec_slave[slave].mbx_l = (unsigned short)(long) text2long(spNode->child->value.opaque);
				 EC_PRINT("Length %4x\n", ec_slave[slave].mbx_l);
				}
		}
        //Read size and address of the input mailbox
		spSend=mxmlFindElement(spMailbox, pSlave, "Recv", NULL, NULL, MXML_DESCEND_FIRST); 
        if (spSend != NULL)
        { EC_PRINT("Input Mailbox: ");  
		  spNode=mxmlFindElement(spSend, spMailbox, "Start", NULL, NULL, MXML_DESCEND);
            if (spNode != NULL)
                {ec_slave[slave].mbx_ro = (unsigned short)(long) text2long(spNode->child->value.opaque);
				 EC_PRINT("Start %4x", ec_slave[slave].mbx_ro);
				 ec_slave[slave].SMtype[1] = 2;}	
				spNode=mxmlFindElement(spSend, spMailbox, "Length", NULL, NULL, MXML_DESCEND);
            if (spNode != NULL)
                {
				 ec_slave[slave].mbx_rl = (unsigned short)(long) text2long(spNode->child->value.opaque);	
				 EC_PRINT("Length %4x\n", ec_slave[slave].mbx_rl);
				}
				spNode=mxmlFindElement(spSend, spMailbox, "PollTime", NULL, NULL, MXML_DESCEND);
            if (spNode != NULL)
            {
                ec_slaveMore[slave].cycleMBoxPolling = TRUE;
                ec_slaveMore[slave].cycleMBoxPollingTime = (unsigned short)(long) text2long(spNode->child->value.opaque);
            }
			spNode=mxmlFindElement(spSend, spMailbox, "StatusBitAddr", NULL, NULL, MXML_DESCEND);
            if (spNode != NULL)
            {
                ec_slaveMore[slave].stateMBoxPolling = TRUE;
                ec_slaveMore[slave].slaveAddressMBoxState = (unsigned short)(long) text2long(spNode->child->value.opaque);
            
			}
        }
        //supports Ethernet over EtherCAT
        spNode = mxmlFindElement(spMailbox, pSlave, "Protocol", NULL, NULL, MXML_DESCEND_FIRST);
        if( spNode != NULL )
        {mxml_node_t *element;
            for( element = spNode; element; element = mxmlFindElement(element, spMailbox,"Protocol" ,NULL, NULL,MXML_NO_DESCEND))
            {
                if( strcmp(element->child->value.opaque, "AoE") == 0 )
                    {ec_slave[slave].AoEdetails = TRUE;
					ec_slave[slave].mbx_proto = ECT_MBXPROT_AOE; 
					}
                else if( strcmp(element->child->value.opaque, "EoE") == 0 )
                    {ec_slave[slave].EoEdetails= TRUE;
                     ec_slave[slave].mbx_proto = ECT_MBXPROT_EOE;
					 }
                else if( strcmp(element->child->value.opaque, "CoE") == 0 )
				{
                    ec_slave[slave].CoEdetails= TRUE; //to be modified in TRANSITION IP
                    ec_slave[slave].SoEdetails= TRUE;
 					ec_slave[slave].mbx_proto =ECT_MBXPROT_COE;	
                }
                else if( strcmp(element->child->value.opaque, "FoE") == 0 )
                    {
					 ec_slave[slave].FoEdetails= TRUE;
                     ec_slave[slave].mbx_proto = ECT_MBXPROT_FOE; 
					 }
                else if( strcmp(element->child->value.opaque, "SoE") == 0 )
                    {
					 ec_slave[slave].SoEdetails= TRUE;
                     ec_slave[slave].mbx_proto = ECT_MBXPROT_SOE; 
					 }
            }
		  EC_PRINT("Protocol %4x\n", ec_slave[slave].mbx_proto);
        }
    }

   //Init Cmds 
 	mxml_node_t *spCmdsInit = mxmlFindElement(pSlave, Root, "InitCmds", NULL, NULL, MXML_DESCEND_FIRST); 
	 InitCmdList *SlaveInitCmd;
	 
	 
    if (spCmdsInit != NULL)
    { 
	   CreateListInitCmd(&SlaveInitCmd);
	   spNode=mxmlFindElement(spCmdsInit,pSlave,"InitCmd", NULL, NULL, MXML_DESCEND);
	  if (spNode != NULL)
	  {
        for( element = spNode; element != NULL; element = mxmlFindElement(element, spCmdsInit,"InitCmd" ,NULL, NULL,MXML_NO_DESCEND))
        {
            EcInitCmdDesc *pCmdDesc=ReadECatCmd(element,spCmdsInit);
            if( pCmdDesc )
            {
               ec_slaveMore[slave].initcmdCnt++;
                ec_slaveMore[slave].initcmdLen += SIZEOF_EcInitCmdDesc(pCmdDesc);                
				InsertInitCmd(&SlaveInitCmd, pCmdDesc);
               
                free(pCmdDesc);
            }
        }
	  }	
		ec_slaveMore[slave].pSlaveInitCmd=(InitCmdList *)SlaveInitCmd;	
		InitCmdList *loop=SlaveInitCmd;
   while (loop != NULL)
   { if( (loop->InitCmd.transition & ECAT_INITCMD_I_P))
          {ec_slaveMore[slave].nIPInitCount++;}			  
		  
	  if( (loop->InitCmd.transition & ECAT_INITCMD_P_I))
            {ec_slaveMore[slave].nPIInitCount++;}

	  if( (loop->InitCmd.transition & ECAT_INITCMD_B_I))
             {ec_slaveMore[slave].nBIInitCount++;}

	if( (loop->InitCmd.transition & ECAT_INITCMD_S_I))
            {ec_slaveMore[slave].nSIInitCount++;}

	if( (loop->InitCmd.transition & ECAT_INITCMD_O_I))
           {ec_slaveMore[slave].nOIInitCount++;}

	if( (loop->InitCmd.transition & ECAT_INITCMD_P_S))
          {ec_slaveMore[slave].nPSInitCount++;}

	  if( (loop->InitCmd.transition & ECAT_INITCMD_S_P))
            {ec_slaveMore[slave].nSPInitCount++;}

	  if( (loop->InitCmd.transition & ECAT_INITCMD_S_O))
             {ec_slaveMore[slave].nSOInitCount++;}

	if( (loop->InitCmd.transition & ECAT_INITCMD_O_S))
            {ec_slaveMore[slave].nOSInitCount++;}

	if( (loop->InitCmd.transition & ECAT_INITCMD_O_P))
           {ec_slaveMore[slave].nOPInitCount++;}

    if( (loop->InitCmd.transition & ECAT_INITCMD_I_B))
           {ec_slaveMore[slave].nIBInitCount++;}
	loop=loop->nextCmd;
    }
  
	}
EC_PRINT("Number of Init Commands for transition:\n");
EC_PRINT("Transition I_P: %d\n",ec_slaveMore[slave].nIPInitCount);
EC_PRINT("Transition P_I: %d\n",ec_slaveMore[slave].nPIInitCount);
EC_PRINT("Transition B_I: %d\n",ec_slaveMore[slave].nBIInitCount);
EC_PRINT("Transition S_I: %d\n",ec_slaveMore[slave].nSIInitCount);
EC_PRINT("Transition O_I: %d\n",ec_slaveMore[slave].nOIInitCount);
EC_PRINT("Transition P_S: %d\n",ec_slaveMore[slave].nPSInitCount);
EC_PRINT("Transition S_P: %d\n",ec_slaveMore[slave].nSPInitCount);
EC_PRINT("Transition S_O: %d\n",ec_slaveMore[slave].nSOInitCount);
EC_PRINT("Transition O_S: %d\n",ec_slaveMore[slave].nOSInitCount);
EC_PRINT("Transition O_P: %d\n",ec_slaveMore[slave].nOPInitCount);
EC_PRINT("Transition I_B: %d\n",ec_slaveMore[slave].nIBInitCount);

 ec_slaveMore[slave].cInitCmds =INITCMD_INACTIVE;   
if (spMailbox != NULL)
 {
      ec_slaveMore[slave].nIPMbxCount=0;
      ec_slaveMore[slave].nPSMbxCount=0;
	   InitMboxCmdList *SlaveMboxInitCmd;
 	  SlaveMboxInitCmd=malloc(sizeof(InitMboxCmdList));
	  SlaveMboxInitCmd=NULL; 
	   
	   mxml_node_t *spCmdsCAN = mxmlFindElement(spMailbox, pSlave, "CoE", NULL, NULL, MXML_DESCEND);
       if (spCmdsCAN != NULL)
        { spNode=mxmlFindElement(spCmdsCAN,spMailbox,"InitCmds", NULL, NULL, MXML_DESCEND_FIRST);
		 if (spNode!= NULL)
		 {
	      
		  EcMailboxCmdDesc *pCmdMbxDesc=NULL;
			  element=mxmlFindElement(spNode,spCmdsCAN,"InitCmd","CompleteAccess", "1", MXML_DESCEND_FIRST);
               if (element!= NULL)
			      {pCmdMbxDesc = ReadCANopenCmd(element,spCmdsCAN,1);}
 		       else
			       {element=mxmlFindElement(spNode,spCmdsCAN,"InitCmd",NULL, NULL, MXML_DESCEND_FIRST);
                     if (element!= NULL)
			         {pCmdMbxDesc = ReadCANopenCmd(element,spCmdsCAN,0);}
				    }
                if (element!= NULL)
                 {
				 if(pCmdMbxDesc->transition==ECAT_INITCMD_MBX_INIT)
                       {ec_slaveMore[slave].nIPMbxCount++;}
				   if(pCmdMbxDesc->transition==ECAT_INITCMD_P_S)
				      {ec_slaveMore[slave].nPSMbxCount++;}

				      ec_slaveMore[slave].mboxCmdCnt++;

                  ec_slaveMore[slave].mboxCmdLen += (unsigned short)SIZEOF_EcMailboxCmdDesc(pCmdMbxDesc);
                 
				 InsertMboxInitCmd(&SlaveMboxInitCmd,pCmdMbxDesc);
                 free(pCmdMbxDesc);
                }
		  do
		    { 
			  EcMailboxCmdDesc *pCmdMbxDesc;
			  element=mxmlFindElement(element,spCmdsCAN,"InitCmd","CompleteAccess", "1", MXML_NO_DESCEND);
               if (element!= NULL)
			      {pCmdMbxDesc = ReadCANopenCmd(element,spCmdsCAN,1);}
 		       else
			      { element=mxmlFindElement(element,spCmdsCAN,"InitCmd",NULL, NULL, MXML_NO_DESCEND);
                   if (element!= NULL)
			      {pCmdMbxDesc = ReadCANopenCmd(element,spCmdsCAN,0);}
				  }
               if (element!= NULL)
			    {
                 if(pCmdMbxDesc->transition==ECAT_INITCMD_MBX_INIT)
                       {plist->ec_slave.nIPMbxCount++;}
				   if(pCmdMbxDesc->transition==ECAT_INITCMD_P_S)
				      {plist->ec_slave.nPSMbxCount++;}

				      plist->ec_slave.mboxCmdCnt++;

                  plist->ec_slave.mboxCmdLen += (unsigned short)SIZEOF_EcMailboxCmdDesc(pCmdMbxDesc);
                 
				 InsertMboxInitCmd(&SlaveMboxInitCmd,pCmdMbxDesc);
                 free(pCmdMbxDesc);
                }
            
		   }
		   while (element);
		  }
		
        }
/*
    mxml_node_t *spCmdsSoE = mxmlFindElement(spMailbox, pSlave, "SoE", NULL, NULL, MXML_DESCEND);
    if (spCmdsSoE != NULL)
      { spNode=mxmlFindElement(spCmdsSoE,spMailbox,"InitCmds", NULL, NULL, MXML_DESCEND);
	    spNode=mxmlFindElement(spNode,spCmdsSoE,"InitCmd", NULL, NULL, MXML_DESCEND);
        for( element = spNode; element; element = mxmlFindElement(element, spCmdsSoE,"InitCmd" ,NULL, NULL,MXML_NO_DESCEND))
         {
            EcMailboxCmdDesc *pCmdDesc = (EcMailboxCmdDesc*)malloc(sizeof(EcMailboxCmdDesc));
			pCmdDesc = ReadSoECmd(element,spCmdsSoE);
            if( pCmdDesc )
            {
                ec_slave[slave].more->mboxCmdCnt++;
                ec_slave[slave].more->mboxCmdLen += (unsigned short)SIZEOF_EcMailboxCmdDesc(pCmdDesc);
				InsertMboxInitCmd(&SlaveMboxInitCmd, pCmdDesc);
                free(pCmdDesc);
            }
        }
    }
   mxml_node_t *spCmdsAoE = mxmlFindElement(spMailbox, pSlave, "AoE", NULL, NULL, MXML_DESCEND);
    if (spCmdsAoE != NULL)
    {
	 spNode=mxmlFindElement(spCmdsAoE,spMailbox,"InitCmds", NULL, NULL, MXML_DESCEND);
	  spNode=mxmlFindElement(spNode,spCmdsAoE,"InitCmd", NULL, NULL, MXML_DESCEND);
        for( element = spNode; element; element = mxmlFindElement(element, spCmdsAoE,"InitCmd" ,NULL, NULL,MXML_NO_DESCEND))//element->NextSiblingElement("InitCmd") )
        {
            EcMailboxCmdDesc *pCmdDesc = (EcMailboxCmdDesc*)malloc(sizeof(EcMailboxCmdDesc));
			
            if( pCmdDesc )
            {
                ec_slave[slave].more->mboxCmdCnt++;
                ec_slave[slave].more->mboxCmdLen += (unsigned short)SIZEOF_EcMailboxCmdDesc(pCmdDesc);
				InsertMboxInitCmd(&SlaveMboxInitCmd, pCmdDesc);
                free(pCmdDesc);
            }
        }
    }
*/
   
   ec_slaveMore[slave].pSlaveMailboxCmd=(InitMboxCmdList *)SlaveMboxInitCmd; 
 } 
  
     return 1;
}
//////////////////////////////////////////////////////////////////////////

/************************************************************
Reads the <Frame> node in the ENI XML file and stores the information

@param[IN] pCyclic = pointer to the <Frame> node in the ENI XML file 
@param[IN] Root = pointer to the Root of the ENI XML file

@param[IN] autoIncrAddr = auto incremental address of the current slave

@param[IN] bDcEnabled = TRUE if Distributed clock is enable for the current slave
@param[IN] slave = index of the current slave
@param[out] pCyclicDesc stores the information read from the ENI XML file
***************************************************************/

void SetCyclicCmds(mxml_node_t *pCyclic, mxml_node_t *Root, EcCycDesc *pCyclicDesc)
{
    if( pCyclic== NULL )
        return;

   
    mxml_node_t *pCycleTime = mxmlFindElement(pCyclic, Root, "CycleTime", NULL, NULL, MXML_DESCEND_FIRST);
    uint32 vCycTime = 0;
    if ( pCycleTime != NULL )
    {
        
        vCycTime = (unsigned long) text2long(pCycleTime->child->value.opaque);
		EC_PRINT("CycleTime: %8x\n", vCycTime);
    }

    
   ec_slaveMore[0].configCycTime=vCycTime;

 mxml_node_t *pFrame=mxmlFindElement(pCyclic, Root, "Frame", NULL, NULL, MXML_DESCEND_FIRST);
 
  mxml_node_t *spCmds = mxmlFindElement(pFrame, pCyclic, "Cmd", NULL, NULL, MXML_DESCEND_FIRST);
    if( spCmds == NULL )
	  {
	    EC_PRINT("Unable to read Frame node!\n\r");
        return -1;
      }
   
	pCyclicDesc->state= EC_STATE_PRE_OP|EC_STATE_SAFE_OP|EC_STATE_OPERATIONAL;
	mxml_node_t *spNode;
    mxml_node_t *element;
	CycCmdList *pCycCmdList=NULL;
    for( element = spCmds;element; element =mxmlFindElement(element, pFrame,"Cmd" ,NULL, NULL,MXML_NO_DESCEND))
    {
        EcCmdDesc  *pCmd;
		pCmd=malloc(sizeof(EcCmdDesc));
		memset(pCmd,0,sizeof(EcCmdDesc));
            
            //read command type
			spNode=mxmlFindElement(element, pFrame,"Cmd" ,NULL, NULL,MXML_DESCEND_FIRST);
            pCmd->head.command = (unsigned char) text2uchar(spNode->child->value.opaque);
			EC_PRINT("Found command: %2d\n",pCmd->head.command);
            switch (pCmd->head.command )
            {
            case EC_CMD_LRD:
            case EC_CMD_LWR:
            case EC_CMD_LRW:
                //read logical address
				spNode=mxmlFindElement(element, pFrame,"Addr" ,NULL, NULL,MXML_DESCEND_FIRST);
                pCmd->head.laddr = SWAPDWORD((unsigned long)(long) text2long(spNode->child->value.opaque));
				EC_PRINT("Logic address: %8x\n",pCmd->head.laddr);
                break;
            default:
                //read address page and offset
				spNode=mxmlFindElement(element, pFrame,"Adp" ,NULL, NULL,MXML_DESCEND_FIRST);
                pCmd->head.ADP = SWAP((unsigned short)(long) text2long(spNode->child->value.opaque));
				spNode=mxmlFindElement(element, pFrame,"Ado" ,NULL, NULL,MXML_DESCEND_FIRST);
                pCmd->head.ADO = SWAP((unsigned short)(long) text2long(spNode->child->value.opaque));
				EC_PRINT("ADP: %4x ADO: %4x\n",pCmd->head.ADP, pCmd->head.ADO);
            }
            spNode=mxmlFindElement(element, pFrame,"DataLength" ,NULL, NULL,MXML_DESCEND_FIRST);
            if (spNode != NULL) 
			{
			  pCmd->head.dlength =  ((unsigned short)(long) text2long(spNode->child->value.opaque));
              EC_PRINT("Datalength %4x\n",pCmd->head.dlength);
            }
            //Read the states this command should be sent in.
            mxml_node_t *spStates = mxmlFindElement(element, pFrame,"State" ,NULL, NULL,MXML_DESCEND_FIRST);
            mxml_node_t *element1;
            pCmd->state = 0;
            for( element1 = spStates; element1; element1 = mxmlFindElement(element1, element,"State" ,NULL, NULL,MXML_NO_DESCEND))
            {
                if( strcmp(element1->child->value.opaque, "INIT") == 0 )
                    pCmd->state |= EC_STATE_INIT;
                else if( strcmp(element1->child->value.opaque, "PREOP") == 0)
                    pCmd->state |= EC_STATE_PRE_OP;
                else if( strcmp(element1->child->value.opaque, "SAFEOP") == 0)
                    pCmd->state |= EC_STATE_SAFE_OP;
                else if( strcmp(element1->child->value.opaque, "OP") == 0)
                    pCmd->state |= EC_STATE_OPERATIONAL;
            }
            pCmd->cntSend               = 0;
            //read working counter
			spNode=mxmlFindElement(element, pFrame,"Cnt" ,NULL, NULL,MXML_DESCEND_FIRST);
            if( spNode != NULL )
			{
                pCmd->cntRecv  = (unsigned short)(long) text2long(spNode->child->value.opaque);
                EC_PRINT("Expected WKC %4x\n\r",pCmd->cntRecv);
			}
            pCmd->cmdSize  = sizeof(ec_comt)+sizeof(uint16)+(pCmd->head.dlength);
            
            pCmd->copyInputs  = FALSE;
            pCmd->copyOutputs = FALSE;
            spNode=mxmlFindElement(element, pFrame,"InputOffs" ,NULL, NULL,MXML_DESCEND_FIRST);
            if( spNode !=NULL )
            {   //this command is responsible for reading the input process data (LRD, BRD, LRW)
                pCmd->copyInputs            = TRUE;
                pCmd->imageSize[VG_IN]  = sizeof(ec_comt)+sizeof(uint16)+(pCmd->head.dlength);
                pCmd->imageOffs[VG_IN]  = (unsigned short)(long) text2long(spNode->child->value.opaque);
            }
            spNode=mxmlFindElement(element, pFrame,"OutputOffs" ,NULL, NULL,MXML_DESCEND_FIRST);
            if (spNode !=NULL)
            {  //this command is responsible for writing the output process data (LWR, LRW)
                
                switch ( pCmd->head.command )
                {
                case EC_CMD_LRW:
                case EC_CMD_LWR:
                case EC_CMD_BWR:
                case EC_CMD_FPWR:
                    pCmd->copyOutputs           = TRUE;
                    break;
                default:
                    pCmd->copyOutputs           = FALSE;
                    break;
                }
                pCmd->imageSize[VG_OUT] = sizeof(ec_comt)+sizeof(uint16)+(pCmd->head.dlength);
                pCmd->imageOffs[VG_OUT] = (unsigned short)(long) text2long(spNode->child->value.opaque);
            }

            if( pCmd->copyOutputs || pCmd->copyInputs )
            {
                

                pCyclicDesc->size                     += pCmd->cmdSize;
                pCyclicDesc->cntCmd++;


            }
		 //insert the command in the list
		 
		 InsertCycCmd(&pCycCmdList,pCmd);
		 free(pCmd);
    }
       
		pCyclicDesc->CycCmds=(CycCmdList*)pCycCmdList;
    }
    

///////////////////////////////////////////////////////////////////////////////////////////
/*
*read the ProcessImage node in the XML file and store the information about variables in the two buffers
* VariableOutList and VariableInList
*/
void SetProcImg(mxml_node_t *ProcImg, mxml_node_t *Root)
{
 mxml_node_t *element,*spNode;
 char *szName;
 int LunStr,i,numIn=0,numOut=0;
 
	
 mxml_node_t *Input=mxmlFindElement(ProcImg, Root,"Inputs" ,NULL, NULL,MXML_DESCEND_FIRST);
 if (Input)
  {
    EC_PRINT("Found Input Process Image\n\r");
   mxml_node_t *Variable = mxmlFindElement(Input, ProcImg,"Variable" ,NULL, NULL,MXML_DESCEND_FIRST);
   
   for( element = Variable; element; element = mxmlFindElement(element, Input,"Variable" ,NULL, NULL,MXML_NO_DESCEND))
     {
	  ec_variable *VarDesc;
	  VarDesc=malloc(sizeof(ec_variable));
       memset(VarDesc,0, sizeof(ec_variable));

	   spNode= mxmlFindElement(element, Input,"Name" ,NULL, NULL,MXML_DESCEND);
	    if (spNode)
		 {
		   szName =(char *)(spNode->child->value.opaque);
		   LunStr=strlen(szName);
           i=0;
		   while (szName[i]!='.')
		      {i++;}
		   strncpy(&(VarDesc->Slave),szName, i);
		   EC_PRINT("Terminal: %s\n", VarDesc->Slave);
		   szName+=i+1;
		   LunStr-=i+1;
		   strncpy(&(VarDesc->VarName), szName,LunStr);
           EC_PRINT("Variable: %s\n", VarDesc->VarName);
		  }
		 spNode= mxmlFindElement(element, Input,"DataType" ,NULL, NULL,MXML_DESCEND);
		  if (spNode)
		   {
		    strcpy(&(VarDesc->varType),(char *)(spNode->child->value.opaque));
			EC_PRINT("DataType: %s\n", VarDesc->varType);
		    }
          spNode= mxmlFindElement(element, Input,"BitSize" ,NULL, NULL,MXML_DESCEND);
		  if (spNode)
		   {
		    VarDesc->VarLength=(uint16)(long)text2long(spNode->child->value.opaque);
			EC_PRINT("BitSize: %3d\n", VarDesc->VarLength);
		    }
          spNode= mxmlFindElement(element, Input,"BitOffs" ,NULL, NULL,MXML_DESCEND);
		  if (spNode)
		   {
		    VarDesc->VarOffset=(uint16)(long)text2long(spNode->child->value.opaque);
			EC_PRINT("BitOffset: %4x\n\r", VarDesc->VarOffset);
		    }
         		  
       InsertVariable(&VariableInList, VarDesc);
	   free(VarDesc);
 	   numIn++; 
	 }
    EC_PRINT("Found %d variables in Input Process Image\r\n", numIn);
  }

  mxml_node_t *Output=mxmlFindElement(ProcImg, Root,"Outputs" ,NULL, NULL,MXML_DESCEND_FIRST);
 if (Output)
  {
    EC_PRINT("Found Output Process Image\n\r");
	mxml_node_t *Variable = mxmlFindElement(Output, ProcImg,"Variable" ,NULL, NULL,MXML_DESCEND_FIRST);
   
   for( element = Variable; element; element = mxmlFindElement(element, Output,"Variable" ,NULL, NULL,MXML_NO_DESCEND))
     {
	  ec_variable *VarDesc;
	  VarDesc=malloc(sizeof(ec_variable));
       memset(VarDesc,0, sizeof(ec_variable));

	   spNode= mxmlFindElement(element, Output,"Name" ,NULL, NULL,MXML_DESCEND);
	    if (spNode)
		 {
		   szName =(char *)(spNode->child->value.opaque);
		   LunStr=strlen(szName);
           i=0;
		   while (szName[i]!= '.')
		      {i++;}
		   strncpy(&(VarDesc->Slave),szName, i);
		   EC_PRINT("Terminal: %s\n", VarDesc->Slave);
		   szName+=i+1;
           LunStr-=i+1;
		   strncpy(&(VarDesc->VarName), szName,LunStr);
		   EC_PRINT("Variable: %s\n", VarDesc->VarName);

		  }
		 spNode= mxmlFindElement(element, Output,"DataType" ,NULL, NULL,MXML_DESCEND);
		  if (spNode)
		   {
		    strcpy(&(VarDesc->varType),(char *)(spNode->child->value.opaque));
			EC_PRINT("DataType: %s\n", VarDesc->varType);
		    }
          spNode= mxmlFindElement(element, Output,"BitSize" ,NULL, NULL,MXML_DESCEND);
		  if (spNode)
		   {
		    VarDesc->VarLength=(uint16)(long)text2long(spNode->child->value.opaque);
			 EC_PRINT("BitSize: %3d\n", VarDesc->VarLength);
		    }
          spNode= mxmlFindElement(element, Output,"BitOffs" ,NULL, NULL,MXML_DESCEND);
		  if (spNode)
		   {
		    VarDesc->VarOffset=(uint16)(long)text2long(spNode->child->value.opaque);
			EC_PRINT("BitOffset: %4x\n\r", VarDesc->VarOffset);
		    }
        
       InsertVariable(&VariableOutList, VarDesc);
 	   
	   free(VarDesc);
 	   numOut++;  
	 }
    EC_PRINT("Found %d variables in Otuput Process Image\r\n", numOut);
  } 
 }
/*******************************************************************************************

Main function that reads the ENI XML file and call the functions to create the requested structures to store the information

@return 1 if successful
*********************************************************************************************/
int CreateDevice (void) //Debug: XML passed by string buffer instead from file
//int CreateDevice (const char *strXMLConfig)
{
   mxml_node_t *tree;
    uint16 autoIncrAddr = 0;
  
   EC_PRINT("Create Network from ENI XML file");
/*initialize resources*/
  reset();

/************************
  Load XML file and create tree
  ***********************/
  //FILE *fp;
  //fp = fopen(strXMLConfig, "r");
	
	//create tree
    //	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK); 
	tree=mxmlLoadString(NULL, data_xmlprova_xml, MXML_OPAQUE_CALLBACK); 
    //fclose(fp);
	
	EC_PRINT("XML file loaded\r\n");
	
	mxml_node_t *Root = mxmlFindElement(tree, tree,"Config" ,NULL, NULL, MXML_DESCEND);
    if( Root != NULL )
   {
       EC_PRINT("XML file opened");        
    			ec_slavecount= 0;
                mxml_node_t  *pSlaves = mxmlFindElement(Root, tree,"Slave" ,NULL, NULL, MXML_DESCEND);
                mxml_node_t *element =pSlaves;
				while (element != NULL)
				{ec_slavecount++;
				 element = mxmlFindElement(element, Root,"Slave" ,NULL, NULL, MXML_NO_DESCEND);
				 }
	    EC_PRINT("Found %d slaves in XML file\r\n", ec_slavecount); 
	
			
			
	  //find the node "master"
	 mxml_node_t *nodeMaster;
    
      nodeMaster = mxmlFindElement(tree, tree,"Master",NULL, NULL, MXML_DESCEND);
	   if (nodeMaster != NULL)
	   {
	     EC_PRINT("Start Master configuration\r\n");
		  
 		if (!CreateMaster(nodeMaster,ec_slavecount))
             {	EC_PRINT("Configuration of Mater's node failed!\r\n"); 	
		       return -1;}
	    }
	   else
	    {  
		  EC_PRINT("Unable to find Mater's node!\r\n"); 
	      return -1;
	    }
		
	 mxml_node_t *pNode;
	 
     //Create a Slave for each <Slave> tag <Config>.
	  int NumSlave=1;
     for( pNode = pSlaves; pNode; pNode = mxmlFindElement(pNode, Root,"Slave" ,NULL, NULL,MXML_DESCEND))
       {
                    if( pNode )
                    {
                        boolean bDcEnable = FALSE;
						EC_PRINT("Start Slave %d configuration\r\n", NumSlave);
                        CreateSlave(pNode, Root, autoIncrAddr, &bDcEnable, NumSlave);
						autoIncrAddr--;
						NumSlave++;
                       
                    }
        }
		
				 	
		//Read Cyclic commands from XML file and create a list
		pNode = mxmlFindElement(Root, tree,"Cyclic" ,NULL, NULL,MXML_DESCEND);
                if( pNode )
                {
                    EC_PRINT("Start Cyclic commands configuration\r\n");
 				    SetCyclicCmds(pNode, Root, &Cyclic);	
                }
		pNode=mxmlFindElement(Root, tree,"ProcessImage" ,NULL, NULL, MXML_DESCEND);
		    if ( pNode )
			    {
				 EC_PRINT("Start Process Image configuration\r\n");
				 SetProcImg(pNode, Root);
				}
				
 	return 1;  
	}
	 else 
	  {EC_PRINT("failed in opening XML file\r\n");
	  return -1;}
	  
  }
 
 
