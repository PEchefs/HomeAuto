
#define MAX_NUM_SENSORS   4
#define MAX_NUM_SWITCHES  4

typedef struct
{
  byte RFNodeId;
  byte RFNetworkId;
  int  RFMemAddr;
  byte RFMasterId;
}gtype_RFId_st;

typedef struct 
{
  boolean Status;  // Alive or dead in the network - Value after installation - dead. Alive only after pairing with a master. When dead, only request it will process is a pair request
  long NodeID;          // Equivalent to RF ID
  long MasterID;
  int  NodeMemAddr;
}gtype_Node_st;

typedef struct
{
  int    SensorId;
  char   SensorType;
  int    SensorValue;
  byte   SensorPin;
  byte   SensorStatus;
  int    SensorMemAddr;
}gtype_Sensor_st;

typedef struct
{
  int   SwitchId;
  int   SwitchValue;
  byte  SwitchPin;
  int   SwitchMemAddr;
  byte  SwitchStatus;
  gtype_Sensor_st SwitchSenor;
  
}gtype_Switch_st;

gtype_Switch_st g_Switches_st[01]={
  
(222, 234,0x00,0xff),   
};

typedef union
{
  struct
  {
  int NodeId;
  int MasteId;
  int SwitchId;
  int SensorType;
  int  Value;  
  }
  byte array[];
};

boolean NodeInit()
{
  //retrieve from memory and set the structure,switches and sensor status
}

boolean RF_Init()
{
  //Initialize RF
}

boolean TransmitData()
{
  //Read Global Variables and send to RF
  //Acknowledge the ACK signal
}

boolean SetStatus()
{
 //Set the Switches and Sensors
 //Write to EEPROM
}
boolean GetStatus()
{
 
}

boolean ReceiveData()
{
  
}

boolean Process()
{
  
}

byte ReadMem(byte Addr)
{
  
}

byte WriteMem(byte Addr)
{
  
}

boolean Manual()
{
  
}

boolean Hardware()
{
  
}

void setup
{
}

void loop
{
}

