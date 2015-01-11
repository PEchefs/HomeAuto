#include <SPI.h>
#include <RFM69.h>

#define SENDERID       2    //unique for each node on same network  
#define NETWORKID     100
#define FREQUENCY   RF69_433MHZ

// Singleton instance of the radio
RFM69 rf69;
 
unsigned int g_StartByte_u16 = 0x7771; //Start Pattern
unsigned int g_EndByte_u16 = 0x7177; //End Pattern
unsigned long g_TimeStamp1_u16 = 0;  //To Calculate Timeout when receiving from PI
boolean g_StringComplete_b = false;  // Variable to check whether data reception is complete from PI
byte g_IncomingByte_byt;  
byte g_SendtoPi_byt[8];  //Array of bytes to be sent back to PI
byte g_RecvNodeMsg_ui8[10]; //Array of bytes Received from Node

union
{
  byte g_RecByte_byt[17];
  struct
  {
    unsigned int g_RecStartByte_u16;
    byte  g_RFId_byt;
    byte  g_ActualMsg_byt[12];
    unsigned int g_RecEndByte_u16;
  };
  
}g_ReceivedByte_un;


void g_ReceiveData_vd(void)
{
  byte l_Len_byt;
  byte l_Index_ui8;
  for(l_Index_ui8 = 0;l_Index_ui8< rf69.DATALEN;l_Index_ui8++)
  {
    g_RecvNodeMsg_ui8[l_Index_ui8]=rf69.DATA[l_Index_ui8];
    //Serial.println(g_RecvNodeMsg_ui8[l_Index_ui8], HEX);
  }
  //Serial.println("Received RAW RF Data:");
  if (rf69.ACKRequested())
  {
    rf69.sendACK();
   // Serial.println("ACK sent");
  }
 return; 
}

int g_MatchId_u16(void)
{
  if ((g_RecvNodeMsg_ui8[0]<<8 | g_RecvNodeMsg_ui8[1]) == (g_ReceivedByte_un.g_ActualMsg_byt[7]<< 8 | g_ReceivedByte_un.g_ActualMsg_byt[6]))
  {
    if ((g_RecvNodeMsg_ui8[2]<<8 | g_RecvNodeMsg_ui8[3]) == (g_ReceivedByte_un.g_ActualMsg_byt[5]<< 8 | g_ReceivedByte_un.g_ActualMsg_byt[4]))
    {
      return 1;
    }
  }
  return 0;
}
void g_SendtoPi_vd(void)
{
  if (g_SendtoPi_byt[0] != 0)
  {
    g_SendtoPi_byt[1] = g_RecvNodeMsg_ui8[4];
    g_SendtoPi_byt[2] = g_RecvNodeMsg_ui8[5];
    g_SendtoPi_byt[3] = g_RecvNodeMsg_ui8[6];
    g_SendtoPi_byt[4] = 0;
    g_SendtoPi_byt[5] = 0;
    g_SendtoPi_byt[6] = rf69.RSSI;
    g_SendtoPi_byt[7] = 1;
  }
  else
  {
    for (int Index = 1; Index < 8; Index++)
    {
      g_SendtoPi_byt[Index]=0;
    }
   // Serial.println("Time Out");
  }
  for (int Index = 0; Index < 8; Index++)
  {
     // Serial.println(g_SendtoPi_byt[Index]);
  }

  Serial.write(g_SendtoPi_byt, 8);  
}

void g_Rfinit_vd(void)
{
  Serial.begin(115200);
  rf69.initialize(FREQUENCY,SENDERID,NETWORKID);  
   // Serial.println("Operating at 433MHZ");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  rf69.setHighPower();
}
void setup()
{
  g_Rfinit_vd();
}

void loop()
{
  if (g_StringComplete_b)
  {
    g_StringComplete_b = false;
    if ((g_StartByte_u16 == g_ReceivedByte_un.g_RecStartByte_u16) && (g_EndByte_u16 == g_ReceivedByte_un.g_RecEndByte_u16))
    {
      
      if(rf69.sendWithRetry(g_ReceivedByte_un.g_RFId_byt, g_ReceivedByte_un.g_ActualMsg_byt, sizeof(g_ReceivedByte_un.g_ActualMsg_byt), 5, 45))
      {
        // Serial.println("Sent Successfully");
      }
      else
      {
        g_SendtoPi_byt[0] = 0;
      //  g_SendtoPi_vd();
      }
    }
    
    unsigned long l_LoopTime_u16 = millis();
    unsigned long l_LoopTime2_u16 = 0;
    boolean l_DataRec_b = 1;
    while(rf69.receiveDone() == 0)
    {
      l_LoopTime2_u16 = millis();
      
      if ((l_LoopTime2_u16 - l_LoopTime_u16) > 300 )
      {
        l_DataRec_b = 0;
        g_SendtoPi_byt[0] = 0;
        g_SendtoPi_vd();
        break;
      }      
    }
   
    if(l_DataRec_b)
    {
     // Serial.println("Data Received");
      g_ReceiveData_vd();
      g_SendtoPi_byt[0] = g_MatchId_u16();
      g_SendtoPi_vd();
    }      
  }
}

void serialEvent() 
{
  static int ls_Index_u16 = 0;
  
  if (ls_Index_u16 != 0)
  {
    unsigned long l_TimeStamp2_u16 = millis();
    if ((l_TimeStamp2_u16 - g_TimeStamp1_u16) > 15)
    ls_Index_u16 = 0;
  }  
  
  while (Serial.available()) 
  {
    g_IncomingByte_byt = (char)Serial.read();
    g_ReceivedByte_un.g_RecByte_byt[ls_Index_u16] = g_IncomingByte_byt;
    //Serial.print("Assigned Data:  ");Serial.println(g_ReceivedByte_un.g_RecByte_byt[ls_Index_u16], HEX);
    //Serial.println(ls_Index_u16);
    ls_Index_u16++;
    if (ls_Index_u16 == 17)
    {
      g_StringComplete_b = true;
      //Serial.println("g_StringComplete_b: ");//Serial.println(g_StringComplete_b);
      ls_Index_u16 = 0;
      delay(20);
    }
  }
  g_TimeStamp1_u16 = millis();
}
