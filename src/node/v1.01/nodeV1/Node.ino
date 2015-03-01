
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
#include <EEPROM.h>
#include <RFM69.h>

/***************************************************/
/**********Preprocessor Derivatives*****************/

#define MAX_NUM_SENSORS   	1
#define MAX_NUM_SWITCHES  	2
#define SENDERID       		2    //unique for each node on same network
#define RECEIVEID      		97
#define NETWORKID     		100
#define FREQUENCY   		RF69_915MHZ
#define ENCRYPTKEY              "NFSUndergroundto"
#define DIMFEATURE              0
/***************************************************/
/************Global Data Types**********************/
typedef struct
{
  byte RFNodeId;
  byte RFNetworkId;
  int  RFMemAddr;
  byte RFMasterId;
}
gtype_RFId_st;

typedef struct 
{
  boolean       Status;  // Alive or dead in the network - Value after installation - dead.
  unsigned long NodeId;  // Equivalent to RF ID
  unsigned long MasterId;
  byte          MemAddr;
}
gtype_Node_st;

typedef struct
{
  int       Id;
  char      Type;
  int       Value;
  byte      Pin;
  byte      Status;
  byte      MemAddr;
}
gtype_Sensor_st;

typedef struct
{
  unsigned int Id;
  byte         Value;
  byte         OutPin;
  byte         InPin;
  byte         MemAddr;
  byte         Status;
  byte         lock;
  byte         Dimlock;   
}
gtype_Switch_st;

/***************************************************/
/************Global Variables Declarations**********/

RFM69 rf69;
gtype_Node_st Node;
gtype_Switch_st Switch[MAX_NUM_SWITCHES];
unsigned short  g_SwitchDimVal_u8[MAX_NUM_SWITCHES];
int             g_SendTime_u16;
int             g_Retries_u16;
byte            g_IncomingByte_byt[32];
byte            g_SendByte_u8[12];
unsigned int    g_Command_u16;
unsigned int    g_RecSwitchId_u16;

union
{
  struct
  {
    unsigned int  Command;
    unsigned int  SwitchId;
    unsigned long NodeId;
    unsigned long MasterId;
  };
  byte g_ReceivedByte_byt[12];
}
g_RecId_un;

/***************************************************/
/************EEPROM and Pin Initialisations**********/

const unsigned short int gem_SwitchAddr_u8[MAX_NUM_SWITCHES]={20,30};
const unsigned short int gem_SwitchLockStatus_u8[MAX_NUM_SWITCHES]={21,31};
const unsigned short int gem_SwitchDimLockStatus_u8[MAX_NUM_SWITCHES]={22,32};
const unsigned short int gem_SwitchDimValue_u8[MAX_NUM_SWITCHES]={23,33};
const unsigned short int g_SwitchOutPins_u8[MAX_NUM_SWITCHES]={5,6};
const unsigned short int g_SwitchInPins_u8[MAX_NUM_SWITCHES]={14,15};
const unsigned int g_SwitchId_u16[MAX_NUM_SWITCHES]={0x7265,0x7974};


/***************************************************/
/************Global Function Declarations**********/
void g_RfInit_vd(void);         //To Initialise the RF node. To be run at void setup()
void g_MemAddrInit_vd(void);    //To assign the address of Nodes, Swtiches and Pins. Declare the pins as Input/Output.To be run at void setup()
void g_NodeInit_vd(void);       //To initialise the node with Master and Node address when its dead. To be run only once when node is dead.
void g_ReadSwStatus_vd(void);   //To read the previous switch status from EEPPROM upon power/soft reset.
void g_SerialReceive_vd(void);  //Just for testing purpose. To be removed later.
void g_ReceiveData_vd(void);    //To receive data from server through rf.
void g_MatchId_vd(void);        //To verify the master id, node id and switch id and the command to execute.
void g_ProcessData_vd(byte f_SwIndex_u8, int f_Command_u16);//To process the commands received from server for individual switches.
void g_GetStatus_vd(void);      //To Read the status of the switches before sending to the Server
void g_SendStatus_vd(void);     //To Send back the status of the switches to the server through RF
void g_Manual_vd(void);         //To process the local manual switching and change the status.
void g_ClearAll_vd(void);

/***************************************************/
/************Global Function Definitions**********/

/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: RF Module Initialisation at Power ON
*
*******************************************************/
void g_RfInit_vd(void)
{
  Serial.begin(115200);
  rf69.initialize(FREQUENCY,RECEIVEID,NETWORKID);
  Serial.println("Operating at 433MHZ");
  //Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  rf69.setHighPower();
  rf69.encrypt(ENCRYPTKEY);
}

/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: Assigning EEPROM address, Switch ID's,
*             Pin Initialisations
*******************************************************/
void g_MemAddrInit_vd(void)
{
  byte l_Index_u8;
  Node.MemAddr = 1;
  for (l_Index_u8=0; l_Index_u8 < MAX_NUM_SWITCHES; l_Index_u8++)
  {
    Switch[l_Index_u8].MemAddr = gem_SwitchAddr_u8[l_Index_u8];//Assign the values from global variables for Inpin,Outpin,Id and Address 
    Switch[l_Index_u8].InPin = g_SwitchInPins_u8[l_Index_u8];
    Switch[l_Index_u8].OutPin = g_SwitchOutPins_u8[l_Index_u8];
    Switch[l_Index_u8].Id = g_SwitchId_u16[l_Index_u8];
    Switch[l_Index_u8].lock = 0; 
    Switch[l_Index_u8].Status = 0; //Assuming all the switches are OFF upon reset.
    Switch[l_Index_u8].Dimlock = 0;
    
    if(EEPROM.read(gem_SwitchDimLockStatus_u8[l_Index_u8]) == 1)
    {
      Switch[l_Index_u8].Dimlock = 1;
      g_SwitchDimVal_u8[l_Index_u8] = EEPROM.read(gem_SwitchDimValue_u8[l_Index_u8]);
    }
    else
    {
      g_SwitchDimVal_u8[l_Index_u8] = 255;
    }
    
    pinMode(Switch[l_Index_u8].OutPin,OUTPUT); //Initialise pinModes 
    pinMode(Switch[l_Index_u8].InPin,INPUT);
    digitalWrite(Switch[l_Index_u8].InPin,HIGH);
    
    Switch[l_Index_u8].Value = (digitalRead(Switch[l_Index_u8].InPin));//Assign the Initial Switch.Value  

    Serial.print("Switch Memaddress:");
    Serial.println(Switch[l_Index_u8].MemAddr);
    Serial.print("Switch InPin:");
    Serial.println(Switch[l_Index_u8].InPin);
    Serial.print("Switch OutPin:");
    Serial.println(Switch[l_Index_u8].OutPin);
    Serial.print("Switch Id:");
    Serial.println(Switch[l_Index_u8].Id, HEX);
  }
  return;
}
/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: Initialisation of Node at Dead State
*
*******************************************************/
void g_NodeInit_vd(void)
{
  union
  {
    byte l_Id_u8[4];
    unsigned long l_Id_ui32;
  }l_Id_un;
  
  byte index;
  
  l_Id_un.l_Id_ui32 = Node.NodeId;                    //Read assigned NodeId 
  
  for(index = 0; index < 4; index++)
  {
    EEPROM.write(index+6, l_Id_un.l_Id_u8[index]);  //Writing NodeId at location 6 to 9 in EEPROM
    l_Id_un.l_Id_u8[index] = g_RecId_un.g_ReceivedByte_byt[index+4];  //Writing Received NodeId to local variable to compare in next if statement  
  }
  
  if ((Node.NodeId == l_Id_un.l_Id_ui32) && (g_RecId_un.Command == 0x6473))
  {
    for(index = 0; index < 4; index++)
    {
      l_Id_un.l_Id_u8[index] = g_RecId_un.g_ReceivedByte_byt[index+8]; //MasterId starts at 8th to 11th byte of Incoming Byte
      EEPROM.write(index+2, l_Id_un.l_Id_u8[index]);   //Writing MasterId at location 2 to 5 in EEPROM  
    }
    Node.MasterId = l_Id_un.l_Id_ui32;
    Serial.print("Assigned New Master Id:");
    Serial.println(Node.MasterId,HEX);
    EEPROM.write(Node.MemAddr,10);
    Node.Status=1;
    g_GetStatus_vd();
  }
  else
  {
    Serial.println("Node Id:"); Serial.println(Node.NodeId, HEX);
    Serial.println("Received Node Id which didn't match with existing Node Id:"); Serial.println(l_Id_un.l_Id_ui32, HEX);
  }
    
 return; 
}

/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: Read Master And Node Id from EEPROM upon
*             Power reset
*******************************************************/
void g_NodeMemRead_vd(void)
{
  union
  {
    byte l_Id_u8[4];
    unsigned long l_Id_ui32;
  }
  l_Id_un;
  
  byte l_Index_u8;
  
  int val = EEPROM.read (Node.MemAddr);
  Serial.print("The value at Node MemAddress:");
  Serial.println(val);
  
  for(l_Index_u8 = 0; l_Index_u8 < 4; l_Index_u8++)
  {
    l_Id_un.l_Id_u8[l_Index_u8] = EEPROM.read(l_Index_u8+2); //Master Id address is stored at EEPROM location 2 
  }
  Node.MasterId = l_Id_un.l_Id_ui32;
  Serial.print("Assigned Master Id from EEPROM:");
  Serial.println(Node.MasterId,HEX);

  for(l_Index_u8 = 0; l_Index_u8 < 4; l_Index_u8++)
  {
    l_Id_un.l_Id_u8[l_Index_u8] = EEPROM.read(l_Index_u8+6); //Node Id address is stored at EEPROM location 6  
  }

  Node.NodeId = l_Id_un.l_Id_ui32;
  Serial.print("Assigned Node Id from EEPROM:");
  Serial.println(Node.NodeId,HEX);
  
  return;
}
/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: Receiving Data Serially
*
*******************************************************/
void g_SerialReceive_vd(void)
{
  // read the incoming byte:
  g_IncomingByte_byt[0] = Serial.read();
  // say what you got:
  Serial.print("Serially Received Data: ");
  Serial.println(g_IncomingByte_byt[0]);
  
  if(g_IncomingByte_byt[0]=='z')
  {
    g_ClearAll_vd();
  }
  return;
}

/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: Receive Data through RF from Server
*
*******************************************************/
void g_ReceiveData_vd(void)
{
  Serial.println("Enter the Receive done function");
  byte l_Len_byt;
  byte l_Index_u8;
  for(l_Index_u8 = 0;l_Index_u8< rf69.DATALEN;l_Index_u8++)
  {
    g_RecId_un.g_ReceivedByte_byt[l_Index_u8]=rf69.DATA[l_Index_u8];
   /* Serial.print(g_RecId_un.g_ReceivedByte_byt[l_Index_u8],HEX);
    if(l_Index_u8 == 3 || l_Index_u8 == 7 || l_Index_u8 == 11)
    Serial.println(""); */
  }
  
  if (rf69.ACKRequested())
  {
    rf69.sendACK();
    Serial.println("ACK sent");
  }
  Serial.println("Received RAW RF Data:");
 return; 
}

void g_MatchId_vd(void)
{
  
  Serial.print("Received MasterId:");
  Serial.println(g_RecId_un.MasterId, HEX);
  
  Serial.print("Received NodeId:");
  Serial.println(g_RecId_un.NodeId, HEX);
  
  Serial.print("Received SwitchId:");
  Serial.println(g_RecId_un.SwitchId, HEX);
  
  Serial.print("Received Command:");
  Serial.println(g_RecId_un.Command, HEX);
  
  return;
 }
 
/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: To process the received Command for a Switch
*
*******************************************************/
 void g_ProcessData_vd(byte f_SwIndex_u8, int f_Command_u16)
 {
   if(f_Command_u16 == 0x6166)
   {    
     g_GetStatus_vd();
   }  
   else
   {
     if(lowByte(f_Command_u16) == 0x61)
     {
       if (DIMFEATURE)
       {
         analogWrite(Switch[f_SwIndex_u8].OutPin, highByte(f_Command_u16));
         g_SwitchDimVal_u8[f_SwIndex_u8] = highByte(f_Command_u16);
         EEPROM.write(gem_SwitchDimValue_u8[f_SwIndex_u8], g_SwitchDimVal_u8[f_SwIndex_u8]);
         Switch[f_SwIndex_u8].Status = 1;
         EEPROM.write(Switch[f_SwIndex_u8].MemAddr, Switch[f_SwIndex_u8].Status);
       }
       else
       {
         digitalWrite(Switch[f_SwIndex_u8].OutPin, HIGH);
         Switch[f_SwIndex_u8].Status = 1;
         EEPROM.write(Switch[f_SwIndex_u8].MemAddr, Switch[f_SwIndex_u8].Status);
       }
     }
     if(f_Command_u16 == 0x6164)
     {
       digitalWrite(Switch[f_SwIndex_u8].OutPin, LOW);
       Switch[f_SwIndex_u8].Status = 0;
       EEPROM.write(Switch[f_SwIndex_u8].MemAddr, Switch[f_SwIndex_u8].Status);
     } 
     if(f_Command_u16 == 0x787A)
     {
       Switch[f_SwIndex_u8].lock = 1;
       EEPROM.write(gem_SwitchLockStatus_u8[f_SwIndex_u8], 1);
     }
     if(f_Command_u16 == 0x637A)
     {
       Switch[f_SwIndex_u8].lock = 0;
       EEPROM.write(gem_SwitchLockStatus_u8[f_SwIndex_u8], 0);
     }
     
     if (DIMFEATURE)
     {
       if(f_Command_u16 == 0x6867) 
       {
         Switch[f_SwIndex_u8].Dimlock = 1;
         EEPROM.write(gem_SwitchDimLockStatus_u8[f_SwIndex_u8], 1);  
       }
       if(f_Command_u16 == 0x6A67) 
       {
         Switch[f_SwIndex_u8].Dimlock = 0;
         EEPROM.write(gem_SwitchDimLockStatus_u8[f_SwIndex_u8], 0);  
       }
     }
     g_GetStatus_vd();
   }
   return;
 }

/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: To Send the data back to Server through RF
*
*******************************************************/ 
 void g_GetStatus_vd(void)
 {
   byte l_Status_byt=0;
   g_SendByte_u8[0]=0;
   g_SendByte_u8[1]=0;
   
   for(uint8_t Index = 0; Index < MAX_NUM_SWITCHES; Index++)
   {
      l_Status_byt = EEPROM.read(Switch[Index].MemAddr);
      g_SendByte_u8[0] |= (l_Status_byt << ((7-Index)-Index));
      l_Status_byt = Switch[Index].lock;
      g_SendByte_u8[0] |= (l_Status_byt << ((7-Index)-(Index+1)));
   }
   Serial.println(g_SendByte_u8[0],HEX);
   g_SendStatus_vd();
   
   return;
 }
   
 
/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: To Send the data back to Server through RF
*
*******************************************************/
void g_SendStatus_vd(void)
{   
  for (byte l_Index_byt=2; l_Index_byt < 12; l_Index_byt++ )
  {
    g_SendByte_u8[l_Index_byt] = g_RecId_un.g_ReceivedByte_byt[l_Index_byt];
  }  
  if(rf69.sendWithRetry(2, g_SendByte_u8, sizeof(g_SendByte_u8), 5, 40))
  {
    Serial.println("Status from node sent Successfully and received ack");
  }
  else
  {
    Serial.println("Status from node sent but Ack from Server not Received");
  }
  return;
}

/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: Read the Switch status upon Power reset
*
*******************************************************/
void g_ReadSwStatus_vd(void)
{
  byte l_SwIndex_u8;
  byte l_Status_u8;
  for(l_SwIndex_u8=0; l_SwIndex_u8 < MAX_NUM_SWITCHES; l_SwIndex_u8++)
  {
    l_Status_u8 = EEPROM.read(Switch[l_SwIndex_u8].MemAddr);
    if(l_Status_u8 && DIMFEATURE)
    {
      if (Switch[l_SwIndex_u8].Dimlock)
      {
        analogWrite(Switch[l_SwIndex_u8].OutPin, g_SwitchDimVal_u8[l_SwIndex_u8]);
      }
      else
      {
        digitalWrite(Switch[l_SwIndex_u8].OutPin, l_Status_u8);
      }
    }
    else
    {
      digitalWrite(Switch[l_SwIndex_u8].OutPin, l_Status_u8);
    }      
    Switch[l_SwIndex_u8].Status = l_Status_u8;
    Switch[l_SwIndex_u8].lock = EEPROM.read(gem_SwitchLockStatus_u8[l_SwIndex_u8]);
  }
  return;
}	

void g_ClearAll_vd(void)
{
  EEPROM.write(Node.MemAddr, 255);
  Node.Status=0; 
  
  return;
}
/*****************************************************
*          PhotoElectricChefs
*Chef: Mukesh
*Date: 12-12-2014
*Description: To check the changes in local Switch Status 
*
*******************************************************/
void g_Manual_vd(void)
{
  for(int l_Index_u8=0; l_Index_u8 < MAX_NUM_SWITCHES; l_Index_u8++)
  {
    int count=0;
   
    int val=digitalRead(Switch[l_Index_u8].InPin);
    
    if(Switch[l_Index_u8].lock)
    Switch[l_Index_u8].Value = (digitalRead(Switch[l_Index_u8].InPin));
    
    if ((val != Switch[l_Index_u8].Value) && (!Switch[l_Index_u8].lock))
    {  
      Serial.println("The switch has been toggled");  
      for(int j=0;j<550;j++)
      {
        if(val==digitalRead(Switch[l_Index_u8].InPin))
        count++;
      }
      if (count > 540)
      {
        Serial.println("The debouncing has matured");
        Switch[l_Index_u8].Value = val;
        if(Switch[l_Index_u8].Status == 1) 
        {
          Switch[l_Index_u8].Status = 0;
          digitalWrite(Switch[l_Index_u8].OutPin, LOW);
         // Serial.println("The Switch is in low state");
        }
        else if(Switch[l_Index_u8].Status == 0)
        {
          if(!Switch[l_Index_u8].Dimlock || (!DIMFEATURE))
          {
            Switch[l_Index_u8].Status = 1;
            digitalWrite(Switch[l_Index_u8].OutPin, HIGH);
           // Serial.println("The Switch is in Digital High state");
          }
          else
          {
            Switch[l_Index_u8].Status = 1;
            analogWrite(Switch[l_Index_u8].OutPin, g_SwitchDimVal_u8[l_Index_u8]);
           // Serial.println("The Switch is in analog high state");
          }
        }
        EEPROM.write(Switch[l_Index_u8].MemAddr, Switch[l_Index_u8].Status);         
      }
    }
    delay(20);
  }    
}   

void setup() 
{
  g_RfInit_vd();
  g_MemAddrInit_vd();
  Node.NodeId = 959985462;
  
  while(Node.Status == 0)
  {
    if(EEPROM.read (Node.MemAddr) != 255)//This if condition is executed only upon power/soft reset.
    {
      g_NodeMemRead_vd();
      g_ReadSwStatus_vd();
      delay(300);
      Node.Status = 1;
    }
    else
    {
      if(rf69.receiveDone())//This if condition executes only when system is initialised for the first time
      {
        g_ReceiveData_vd();
        g_NodeInit_vd(); 
      }
    }
  }
}

void loop()
{
  if (Serial.available() > 0)
  {
    g_SerialReceive_vd();
  }
  if (rf69.receiveDone())
  {
    g_ReceiveData_vd();
    g_MatchId_vd();//Only for debugging purpose. To be removed later

    if(Node.NodeId == g_RecId_un.NodeId) 
    {
      Serial.println("Node Id Matches");
      if (g_RecId_un.Command == 0x6473)
      {
        Serial.println("Master Id change request");
        g_NodeInit_vd();
      }
      else if (Node.MasterId == g_RecId_un.MasterId)
      {
        for (int l_Index_u8=0; l_Index_u8 < MAX_NUM_SWITCHES; l_Index_u8++)
        {
          if(Switch[l_Index_u8].Id == g_RecId_un.SwitchId)
          {
            Serial.print("Switch Id "); Serial.print(g_RecId_un.SwitchId, HEX);Serial.println(" Matched");
            g_ProcessData_vd(l_Index_u8, g_RecId_un.Command);
          }              
        }
      }
      else
      {
        Serial.println("Received Master Id doesn't Match.");
      }
    }
    else
    {
      Serial.println("Received Node Id doesn't Match.");
    }
  }
  g_Manual_vd();
}
