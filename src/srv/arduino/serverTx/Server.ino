/***************************************************/
/**********Header file inclusions*******************/

#include <SPI.h>
#include <RFM69.h>

/***************************************************/
/**********Preprocessor Derivatives*****************/
#define SENDERID       2    
#define NETWORKID     100
#define FREQUENCY   RF69_433MHZ

/***************************************************/
/************Global Data Types**********/

RFM69 rf69;
 
unsigned int g_StartByte_u16 = 0x7771;
unsigned int g_EndByte_u16 = 0x7177;
unsigned long g_TimeStamp1_u16 = 0;
boolean g_StringComplete_b = false;
byte g_IncomingByte_byt;
byte g_ReceivedByte_byt;
byte g_SendtoPi_byt[8];
byte g_SendMsg_ui8[13];

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

/***************************************************/
/************Global Function Definitions**********/
void g_ReceiveData_vd(void)
{
  byte l_Len_byt;
  byte l_Index_ui8;
  for(l_Index_ui8 = 0;l_Index_ui8< rf69.DATALEN;l_Index_ui8++)
  {
    g_SendtoPi_byt[l_Index_ui8+1]=rf69.DATA[l_Index_ui8];
   // Serial.println(g_SendtoPi_byt[l_Index_ui8+1]);
  }
  Serial.println("Received RAW RF Data:");
  if (rf69.ACKRequested())
  {
    rf69.sendACK();
   // Serial.println("ACK sent");
  }
 return; 
}

void g_SendtoPi_vd(void)
{
  if (g_SendtoPi_byt[0] == 1)
  {
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
      //Serial.println(g_SendtoPi_byt[Index]);
    }
  //Serial.println(rf69.RSSI);
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
   // Serial.println(g_ReceivedByte_un.g_RecStartByte_u16);
   // Serial.println(g_StartByte_u16);
   // Serial.println(g_ReceivedByte_un.g_RecEndByte_u16);
   // Serial.println(g_EndByte_u16);
    g_StringComplete_b = false;
    if ((g_StartByte_u16 == g_ReceivedByte_un.g_RecStartByte_u16) && (g_EndByte_u16 == g_ReceivedByte_un.g_RecEndByte_u16))
    {
      // Serial.println("Sending the data now");
      if(rf69.sendWithRetry(g_ReceivedByte_un.g_RFId_byt, g_ReceivedByte_un.g_ActualMsg_byt, sizeof(g_ReceivedByte_un.g_ActualMsg_byt), 5, 45))
      {
        // Serial.println("Sent Successfully");
      }
      else
      {
        g_SendtoPi_byt[0] = 0;
        g_SendtoPi_vd();
      }
    }
    
    unsigned long timestamp = millis();
    boolean l_DataRec_b = 1;
    while(rf69.receiveDone() == 0)
    {
      unsigned long timestamp2 = millis();
      
      if ((timestamp2 - timestamp) > 300 )
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
      delay(60);
      g_SendtoPi_byt[0] = 1;
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
