/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: Node Code for Home Automation
*
*******************************************************/

/***************************************************/
/**********Header file inclusions*******************/

#include <SPI.h>
#include <RFM69.h>

/***************************************************/
/**********Preprocessor Derivatives*****************/

#define SENDERID      2    //unique for each node on same network  
#define NETWORKID     100
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "NFSUndergroundto"

/***************************************************/
/************Global Variables Declarations**********/

RFM69 rf69;
 
byte g_TimeSendRF_u8=0;
byte g_RFTries_u8=0;
unsigned int g_StartByte_u16 = 0x7771; //Start Pattern
unsigned int g_EndByte_u16 = 0x7177; //End Pattern
unsigned long g_PiTimeStamp1_u16 = 0;  //To Calculate Timeout when receiving from PI
unsigned long g_OpStartTime_32;
unsigned long g_RFTimeOut_32;
byte g_IncomingByte_byt;  
byte g_SendtoPi_byt[15];  //Array of bytes to be sent back to PI
byte g_RecvNodeMsg_ui8[20]; //Array of bytes Received from Node

union 
{
  struct
  {
    unsigned int RecRFDataNode:   1;
    unsigned int RecRFData:       1;
    unsigned int ACK_Received:    1;
    unsigned int DataSendtoRF:    1;
    unsigned int SerDataValid:    1;
    unsigned int RecSerDatainTime:1;
    unsigned int RecSerData:      1;
    unsigned int TimeOut:         1;
  };
  byte g_Status_byt;
}g_Status_un;

union
{
  struct
  {
    unsigned long NodeId;
    unsigned long MasterId;
  };
  byte g_RecNode_byt[8];
}
g_DataRecNode_un;


union
{
  byte g_RecByte_byt[17];
  struct
  {
    unsigned int g_RecStartByte_u16;
    byte  g_RFId_byt;
    union
    {
      byte  g_ActualMsg_byt[12];
      struct
      {
        unsigned long CmdSw;
        unsigned long NodeId;
        unsigned long MasterId;
      };
    }SerId;
    unsigned int g_RecEndByte_u16;
  };  
}g_DataRecFromServer_un;


/***************************************************/
/************Global Function Declarations**********/
void g_ReceiveData_vd(void);    //To receive data from Node through rf.
int g_MatchId_u16(void);        //To verify the master and node id
void g_SendtoPi_vd(void);       //To send data back to PI
void g_Rfinit_vd(void);         // To inialise the RF module

/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 24-1-2015
*Description: To receive data from node
*             
*******************************************************/
void g_ReceiveData_vd(void)
{
  byte l_Len_byt;
  byte l_Index_ui8;
  //Serial.println("Received RAW RF Data:");
  for(l_Index_ui8 = 0;l_Index_ui8< rf69.DATALEN;l_Index_ui8++)
  {
    g_RecvNodeMsg_ui8[l_Index_ui8]=rf69.DATA[l_Index_ui8];
   // Serial.println(g_RecvNodeMsg_ui8[l_Index_ui8], HEX);
  }
  if (rf69.ACKRequested())
  {
    rf69.sendACK();
   // Serial.println("ACK sent");
  }
  g_Status_un.RecRFData = 1;
 return; 
}


/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 24-1-2015
*Description: To match the server and node id
*             
*******************************************************/

int g_MatchId_u16(void)
{
  for (int Index = 0; Index < 8; Index++)
  {
    g_DataRecNode_un.g_RecNode_byt[Index] = g_RecvNodeMsg_ui8[Index+4];
  }
  //Serial.println("Received Master Id and Node from Node for Authentication:");
  //Serial.println(g_DataRecNode_un.MasterId, HEX);
  //Serial.println(g_DataRecNode_un.NodeId, HEX);
    
  if (g_DataRecNode_un.MasterId == g_DataRecFromServer_un.SerId.MasterId)
  {
    if (g_DataRecNode_un.NodeId == g_DataRecFromServer_un.SerId.NodeId)
    {
      g_Status_un.RecRFDataNode = 1;
      return 1;
    }
  }
  return 0;
}


/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 24-1-2015
*Description: To send the data back to PI 
*             
*******************************************************/
void g_SendtoPi_vd(void)
{
  unsigned int l_Time_16 = (millis() - g_OpStartTime_32);
  //Serial.print("Time Taken for entire operation:");Serial.println(l_Time_16);
  if (g_Status_un.TimeOut == 0)
  {
    g_SendtoPi_byt[0] = g_Status_un.g_Status_byt;
    g_SendtoPi_byt[1] = g_RecvNodeMsg_ui8[3];
    g_SendtoPi_byt[2] = g_RecvNodeMsg_ui8[2];
    g_SendtoPi_byt[3] = g_RecvNodeMsg_ui8[1];
    g_SendtoPi_byt[4] = g_RecvNodeMsg_ui8[0];
    g_SendtoPi_byt[5] = (l_Time_16/2);
    g_SendtoPi_byt[6] = (g_TimeSendRF_u8/2);
    g_SendtoPi_byt[7] = g_RFTries_u8;
    g_SendtoPi_byt[8] = rf69.RSSI;
  }
  else
  {
    //Serial.println("TimeOut Occured");
    g_SendtoPi_byt[0] = g_Status_un.g_Status_byt;
    g_SendtoPi_byt[1] = 0;
    g_SendtoPi_byt[2] = 0;
    g_SendtoPi_byt[3] = 0;
    g_SendtoPi_byt[4] = 0;
    g_SendtoPi_byt[5] = (l_Time_16/2);
    g_SendtoPi_byt[6] = (g_TimeSendRF_u8/2);
    g_SendtoPi_byt[7] = g_RFTries_u8;
    g_SendtoPi_byt[8] = 0;
  }
  /*Serial.println("Status Byte Data");
  for (int Index=0;Index<8;Index++)
  {
    Serial.println((g_Status_un.g_Status_byt >> (7-Index)) & 0x01);
  }*/
  //Serial.println("Send to PI Data");

  Serial.write(g_SendtoPi_byt,9);
    
  g_Status_un.g_Status_byt=0;
  g_TimeSendRF_u8=0;
  g_RFTries_u8=0;
  for (int Index=0;Index<9;Index++)
  {
    g_SendtoPi_byt[Index]=0;
  }
}

/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 24-1-2015
*Description: To intialise the RF module
*             
*******************************************************/

void g_Rfinit_vd(void)
{
  Serial.begin(115200);
  rf69.initialize(FREQUENCY,SENDERID,NETWORKID);  
  rf69.setHighPower();
  rf69.encrypt(ENCRYPTKEY);
}
void setup()
{
  g_Rfinit_vd();
}

void loop()
{
  if (g_Status_un.RecSerDatainTime)
  {
    //Serial.println("Received Serial Data in time");
    if ((g_StartByte_u16 == g_DataRecFromServer_un.g_RecStartByte_u16) && (g_EndByte_u16 == g_DataRecFromServer_un.g_RecEndByte_u16))
    {
      g_Status_un.SerDataValid = 1;
      
     // Serial.println("Received Serial Data is valid");
      
      g_RFTimeOut_32 = millis();
      while((millis()- g_RFTimeOut_32) < 300)
      {
        if ((!g_Status_un.ACK_Received) && (!g_Status_un.RecRFDataNode))
        {
          g_Status_un.DataSendtoRF=1;
          if(rf69.sendWithRetryData(g_DataRecFromServer_un.g_RFId_byt, g_DataRecFromServer_un.SerId.g_ActualMsg_byt, sizeof(g_DataRecFromServer_un.SerId.g_ActualMsg_byt),&g_TimeSendRF_u8, &g_RFTries_u8, 4, 40))
          {
            g_Status_un.ACK_Received = 1;
           // Serial.println("Sent Successfully To Node");
          }
        }        
        if(rf69.receiveDone())
        {
          g_ReceiveData_vd();
          //Serial.println("Data Received From Node");
          if(g_Status_un.RecRFData)
          {
            g_MatchId_u16();
            
            if(g_Status_un.RecRFDataNode)
            {
              //Serial.println("Master and NodeId Matched");
              g_Status_un.TimeOut=0;
              break;
            }
          }
        }
      }
    }
  }
  if(g_Status_un.g_Status_byt && ((millis() - g_OpStartTime_32) > 30))
  g_SendtoPi_vd();
}

void serialEvent() 
{
  g_Status_un.RecSerData=1;
  g_Status_un.TimeOut=1;
  static int ls_Index_u16 = 0;
  
  if (ls_Index_u16 != 0)
  {
    unsigned long l_TimeStamp2_u16 = millis();
    if ((l_TimeStamp2_u16 - g_PiTimeStamp1_u16) > 20)
    ls_Index_u16 = 0;
  }
  if (ls_Index_u16 == 0)
  {
    g_Status_un.g_Status_byt=0;
    g_Status_un.RecSerData=1;
    g_Status_un.TimeOut=1;
    g_OpStartTime_32 = millis();
  }
  
  while (Serial.available()) 
  {
    g_IncomingByte_byt = (char)Serial.read();
    g_DataRecFromServer_un.g_RecByte_byt[ls_Index_u16] = g_IncomingByte_byt;
    //Serial.print("Assigned Data:  ");Serial.println(g_DataRecFromServer_un.g_RecByte_byt[ls_Index_u16], HEX);
    // Serial.println(ls_Index_u16);
    ls_Index_u16++;
    if (ls_Index_u16 == 17)
    {
      g_Status_un.RecSerDatainTime = 1;
      //Serial.println("g_StringComplete_b: ");Serial.println(g_StringComplete_b);
      ls_Index_u16 = 0;
      delay(40);
    }
  }
  g_PiTimeStamp1_u16 = millis();
}
