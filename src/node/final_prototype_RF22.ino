/*	PhotoElectricChefs	*/
/*    photoelectricchefs.org	*/

/*  NODE CODE  */
/*  Recognized properties and commands(together referred as attributes):
    NDID  -  NODE ID                           -  PROPERTY
    MSID  -  MASTER ID                         -  PROPERTY
    CMID  -  COMMAND ID                        -  PROPERTY
    KPAL  -  KEEP ALIVE(PACKET/ID)             -  PROPERTY
    STST  -  SET STATE                         -  COMMAND
    SDST  -  SEND STATE                        -  COMMAND
    SWMN  -  SWITCH TO MANUAL MODE             -  COMMAND
    SWRM  -  SWITCH TO REMOTE OPERATION MODE   -  COMMAND
    

/************Public Includes************************/
#include <EEPROM.h>
#include <SPI.h>
///#include "RF24.h"
#include <RF22.h>

///RF24 radio(7,8);
RF22 rf22;
//Define number of switches controlled by the node
#define N 4
//Defining pins on Arduino through which switches are controlled

/*
#define SWITCH1_PIN 8
#define SWITCH2_PIN 9
#define SWITCH3_PIN 10
#define SWITCH4_PIN 11
*/

const unsigned short int switch_pins[N]={8,9,10,11};	//for 4 switch node

/* Defining starting addresses in EEPROM where each of the switches' state and/or other properties are stored.
   Assuming 20 bytes would suffice for each switch data. Can extend if needed.
   Here addresses 20-39 : SWITCH1, 40-59 : SWITCH2, 60-79 : SWITCH3, 80-99 : SWITCH4 	*/

/*
#define SWITCH1_EEPROM_START_ADDR 20
#define SWITCH2_EEPROM_START_ADDR 40
#define SWITCH3_EEPROM_START_ADDR 60
#define SWITCH4_EEPROM_START_ADDR 80
*/

const unsigned short int switch_addr[N]={20,30,40,50};	//for 4 switch node 
const unsigned short int commom_info_addr=10;           //starting address to store common information such as NODE_ID,MASTER_ID,LAST_COMMAND_ID,LAST_KEEPALIVE_ID,MANUAL_OVERRIDE

/***************************************************/
/************Private Variables**********************/
unsigned char NODE_ID,MASTER_ID,MANUAL_OVERRIDE=N;
unsigned char NODE_ID_received,MASTER_ID_received,SET_STATE_received,COMMAND_ID_received;
bool NEW_SWITCH_STATE;
unsigned char LIVE_STATE,STATE_DIFFERENCE;
//unsigned char LAST_COMMAND_ID,LAST_KEEPALIVE_ID;

///const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };    //pipes for RF communication
uint8_t pipeno[6];
char data_to_send[33];
char data_to_send_tmp[33];
char rec_val_new[32];    
char attribute[4][10];
char attr_name[4][6];
unsigned char attr_val[4][4];
unsigned short int i,j,k,l,m;
String received_attributes_list[5];
String received_properties[5];
String received_commands[5];
//String received_values_list[5];
unsigned short int num_properties,num_commands;

bool data_to_be_processed=false;

//Every switch has a pin,state and EEPROM start address associated with it
struct switches
{
unsigned short int pin;
boolean state;						//true=ON, false=OFF
byte addr;
};
struct switches S[N];					// switches: S[0],S[1],S[2],...S[N-1]

/***************************************************/
/************Private Function Declarations**********/
void Node_Init();
void RF_Init();
void Read_EEPROM();
void status_update();
void receive_data();
void process_received_data();
unsigned char get_state();
void send_to_server();
void send_data();
void clear_all();
void resetNode();




/***************************************************/
/***********Function Defintions*********************/
void Node_Init()
{	for(i=0;i<N;i++)
	S[i].pin=switch_pins[i];		//
	
	for(i=0;i<N;i++)
	S[i].addr=switch_addr[i];

	for(i=0;i<N;i++)
	pinMode(S[i].pin, OUTPUT);

//Read EEPROM to determine previous states and properties of the switches
	Read_EEPROM(); 		   

//restore switches to previous state(to be done only if MANUAL_OVERRIDE=N)
	for(i=0;i<N;i++)
	status_update(i,S[i].state); 
	//ping to master to be added
}

void RF_Init()
{
///        Serial.begin(57600);
           Serial.begin(9600);        
           Serial.println("ROLE:  SLAVE NODE");
           Serial.println("");

///        radio.begin();
///        radio.setRetries(15,15);
///        radio.openReadingPipe(1,pipes[0]);
///        radio.startListening();
      if (!rf22.init())
        Serial.println("RF22 init failed");
        // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36        
}

void Read_EEPROM()
{
//TEMPORARY ASSIGNMENT FOR NODE_ID AND MASTER_ID
        NODE_ID=0x46;
        MASTER_ID=0x41;
//        NODE_ID=EEPROM.read(commom_info_addr);
//        MASTER_ID=EEPROM.read(commom_info_addr+1);
//        MANUAL_OVERRIDE=EEPROM.read(commom_info_addr+2);
//        LAST_COMMAND_ID=EEPROM.read(commom_info_addr+3);
//        LAST_KEEPALIVE_ID=EEPROM.read(commom_info_addr+4);
	for(i=0;i<N;i++)	
	S[i].state = EEPROM.read(S[i].addr);
//	S[i].id = EEPROM.read(S[i].addr+1); //illustration to read more info abt a particular switch

}



void status_update(unsigned short int switch_number, boolean switch_state)
{
/*Usage Example: status_update(0, true) to turn ON switch S0*/
	
		 digitalWrite(S[switch_number].pin, (switch_state==true)?HIGH:LOW);	//send control signal to switch ON/OFF a switch
		 S[switch_number].state=switch_state;					//update state in switch's element in structure
		 EEPROM.write(S[switch_number].addr, S[switch_number].state);		//update state in EEPROM
		 /*Illustration to write another info regarding S1, say ID into EEPROM:
		 EEPROM.write(S[switch_number].addr+1, S[switch_number].id); 
		 assuming id is an element of switches structure and is defined/passed*/
		
}

void receive_data()
{
    unsigned char rec_val[32];
    unsigned char len=sizeof(rec_val);
/*    bool done = false;
    while (!done)
    {
      done = radio.read( rec_val, 32*sizeof(unsigned char) );
      Serial.print("Received data on Pipe No:");
      Serial.println(pipeno[0]);
      Serial.println("Received data:");
      for(unsigned short int index=0;index<32;index++)
      {
       Serial.print(rec_val[index]);
       rec_val_new[index]=rec_val[index];
      }
      Serial.println("");
      data_to_be_processed=true;
      delay(20);
    }
*/
    if (rf22.recv(rec_val, &len))
    {
      Serial.print("Received data: ");
     /// Serial.println((char*)buf);
      for(unsigned short int index=0;index<32;index++)
      {
       Serial.print(rec_val[index]);
       rec_val_new[index]=rec_val[index];
      }
      Serial.println("");
      data_to_be_processed=true;
      delay(20);
    }
    Serial.println("Done");
    Serial.println("");
}


void process_received_data()
{
  //Separate the attributes received
   
      
       for(i=0,j=0,k=0;i<32;i++)      
       {
       if(rec_val_new[i]==',')
         { //Serial.print("Found ',' at rec_val_new[");Serial.print(i);Serial.println("]");Serial.print("Attribute ");Serial.print(k);Serial.print(" : ");Serial.print(j);Serial.print(" to ");
            l=0;
            while(j<i)
               {
                  if(rec_val_new[j]!=0)
                  attribute[k][l]=rec_val_new[j];
                  l++;
                  j++;    
               }
      //         Serial.println(j);
            j++;  //to skip the ','
            k++;  //to store next attribute in next row of attribute[][]
         }
       }
       
   //Separate attribute type and value
   
   for(k=0;k<4;k++)
   {//Serial.println("");
     for(l=0;l<10;l++)
     {//Serial.print("k=");Serial.print(k);Serial.print(",l=");Serial.println(l);
       if(attribute[k][l]!=0)
         {
           if(attribute[k][l]=='=')
           {//Serial.print("Found '=' at ");Serial.print("k=");Serial.print(k);Serial.print(" l=");Serial.println(l);
             m=0;
             i=0;
             while(m<l)
             {//Serial.print("Copying attribute[");Serial.print(k);Serial.print("][");Serial.print(m);Serial.print("] to attr_name[");Serial.print(k);Serial.print("][");Serial.print(i);Serial.println("]");
               attr_name[k][i]=attribute[k][m];
               i++;
               m++;
             }
             m++;
             j=0;
             while(attribute[k][m]!=0)
             {//Serial.print("Copying attribute[");Serial.print(k);Serial.print("][");Serial.print(m);Serial.print("] to attr_val[");Serial.print(k);Serial.print("][");Serial.print(j);Serial.println("]");
               attr_val[k][j]=attribute[k][m];
               j++;
               m++;
               
             }
           exit;
         }
   }
   
      }
    }
    
    Serial.println();
    Serial.println("Decoded attribute names:");
    for(k=0;k<4;k++)
   {
     for(l=0;l<6;l++)
     {
       if(attr_name[k][l]!=0)
       {//Serial.print("attr_name[");Serial.print(k);Serial.print("][");Serial.print(l);Serial.print("]= "); 
         Serial.print(attr_name[k][l]);
         Serial.print(" ");
         received_attributes_list[k]=received_attributes_list[k]+attr_name[k][l];
       }
     }
     Serial.println();
     Serial.print("[");Serial.print(k);Serial.print("] = ");Serial.println(received_attributes_list[k]);
   }
   
   Serial.println();
    Serial.println("Decoded attribute values:");
    for(k=0;k<4;k++)
   {
     for(l=0;l<4;l++)
     {
       if(attr_val[k][l]!=0)
       {//Serial.print("attr_val[");Serial.print(k);Serial.print("][");Serial.print(m);Serial.print("]= ");
         Serial.print(attr_val[k][l]);
         Serial.print(" ");
 //        received_values_list[k]=String(received_values_list[k]+attr_val[k][l]);
       }
     }
     Serial.println();
  //   Serial.print("received_values_list[");Serial.print(k);Serial.print("] = ");Serial.println(received_values_list[k]);
   }
   
   
  //Infer attribute name and store in a format suitable for comparison
/*  for(i=0,num_properties=0,num_commands=0;received_attributes_list[i]!=0;i++)
  {
    if(received_attributes_list[i] == "NDID" || received_attributes_list[i] == "MSID" || received_attributes_list[i] == "KPAL" || received_attributes_list[i] == "CMID")
    {
      num_properties++;
      received_properties[i]=received_attributes_list[i];
    }
    else if(received_attributes_list[i] == "STST" || received_attributes_list[i] == "SDST" || received_attributes_list[i] == "SWMN" || received_attributes_list[i] == "SWRM")
    {
      num_commands++;
      received_commands[i]=received_attributes_list[i];
    }
  }
*/
   
 for(i=0,j=0,k=0;received_attributes_list[i]!=0;i++)
 {
/* switch(received_attributes_list[i])
 {
   case "MSID": MASTER_ID_received=attr_val[i][0];
                num_properties++;
                break;
   case "NDID": NODE_ID_received=attr_val[i][0];
                num_properties++;
                break;
   case "KPAL"://code to be written
                num_properties++;
                break;
   case "CMID"://code to be written
                num_properties++;
                break;
   case "STST": SET_STATE_received=attr_val[i][0];
                num_commands++;
                break;
   case "SDST"://code to be written
                num_commands++;
                break;
   case "SWMN"://code to be written
                num_commands++;
                break;
   case "SWRM"://code to be written
                num_commands++;
                break;   
 }
*/
  if(received_attributes_list[i] == "MSID")
  {
    received_properties[j]=received_attributes_list[i];
    MASTER_ID_received=attr_val[i][0];
    num_properties++;
    j++;
  }
  else if(received_attributes_list[i] == "NDID")
  {
    received_properties[j]=received_attributes_list[i];
    NODE_ID_received=attr_val[i][0];
    num_properties++;
    j++;
  }
  else if(received_attributes_list[i] == "KPAL")
  {
    //code to be written
    received_properties[j]=received_attributes_list[i];
    num_properties++;
    j++;
  }
  else if(received_attributes_list[i] == "CMID")
  {
    //code to be written
    received_properties[j]=received_attributes_list[i];
    num_properties++;
    j++;
  }
  else if(received_attributes_list[i] == "STST")
  {
    received_commands[k]=received_attributes_list[i];
    SET_STATE_received=attr_val[i][0];
    num_commands++;
    k++;
  }
  else if(received_attributes_list[i] == "SDST")
  {
    //code to be written
    received_commands[k]=received_attributes_list[i];
    num_commands++;
    k++;
  }
  else if(received_attributes_list[i] == "SWMN")
  {
    //code to be written
    received_commands[k]=received_attributes_list[i];
    num_commands++;
    k++;
  }
  else if(received_attributes_list[i] == "SWRM")
  {
    //code to be written
    received_commands[k]=received_attributes_list[i];
    num_commands++;
    k++;
  }
  
 }
  Serial.print("Number of properties received = ");Serial.println(num_properties);
  Serial.print("Number of commands received = ");Serial.println(num_commands);
 
  data_to_be_processed=false;
  
}

unsigned char get_state()
{
  //code to infer state from current sensors to be written
  //temporarily hard-coded for testing
  LIVE_STATE=0x00;      // Binary: 00001000 - Switche S3 is ON, S0, S1 and S2 are off.
  return LIVE_STATE;
  
}

void send_to_server(unsigned short int send_code)
{
  /*  send_code mapping:
      0  -  send state to server
      1  -  
  */   
  switch(send_code)
  {
    case 0:LIVE_STATE=get_state();
      //     data_to_send_tmp[25] = "NDID= ,MSID= ,CMID= ,SDST= ";
           data_to_send_tmp[5]=NODE_ID;
           data_to_send_tmp[12]=MASTER_ID;
           data_to_send_tmp[19]=COMMAND_ID_received;
           data_to_send_tmp[26]=LIVE_STATE;
           break;
  }         
  
}

void send_data(char* data_to_send)
{///       radio.stopListening();
///        radio.openWritingPipe(pipes[0]);
///        bool ok = radio.write( data_to_send, 32*sizeof(char) );  
///        if(ok)
      rf22.send((unsigned char*)data_to_send, sizeof(data_to_send));
      rf22.waitPacketSent();
        Serial.println("Sending ok");
// /       else
///        Serial.println("sending failed");
        //code to send data
 
///        radio.openReadingPipe(1,pipes[1]);
///        radio.startListening();
  
}

void clear_all()
{
  
  //Function to clear transaction specific data like NODE_ID_received, MASTER_ID_received etc
  
  
  
}

void resetNode()
{
 //Function to cater to external reset switch 
}


/***************************************************/


void setup() //Initialization
{
//  attributes=attributes+String("llo");
        attachInterrupt(0, resetNode, RISING);
	Node_Init();	//Initialize node to previous state. 	
        RF_Init();
}

void loop()
{		//Main Loop

      if (rf22.available())
      {
        receive_data();
        if(data_to_be_processed=true)
          process_received_data();
        
        if(MASTER_ID_received == MASTER_ID)
        {Serial.println("Master ID matches");
          if(NODE_ID_received == NODE_ID)
          {Serial.println("NODE ID matches");
            for(i=0;i<num_commands;i++)
            {
              if(received_commands[i] == "STST")
              {
                LIVE_STATE=get_state();
                STATE_DIFFERENCE = LIVE_STATE ^ SET_STATE_received;
                Serial.print("STATE_DIFFERENCE= ");Serial.println(STATE_DIFFERENCE);
                if(STATE_DIFFERENCE == 0x01)
                {
                  NEW_SWITCH_STATE=((SET_STATE_received&0x01)==0x01)?true:false;
                  Serial.print("New state of switch S0 is ");Serial.println(NEW_SWITCH_STATE);
                  status_update(0,NEW_SWITCH_STATE);
                }
                else if(STATE_DIFFERENCE == 0x02)
                {
                  NEW_SWITCH_STATE=((SET_STATE_received&0x02)==0x02)?true:false;
                  Serial.print("New state of switch S1 is ");Serial.println(NEW_SWITCH_STATE);
                  status_update(1,NEW_SWITCH_STATE);
                }
                else if(STATE_DIFFERENCE == 0x04)
                {
                  NEW_SWITCH_STATE=((SET_STATE_received&0x04)==0x04)?true:false;
                  Serial.print("New state of switch S2 is ");Serial.println(NEW_SWITCH_STATE);
                  status_update(2,NEW_SWITCH_STATE);
                }
                else if(STATE_DIFFERENCE == 0x08)
                {
                  Serial.print("SET_STATE_received=");Serial.println(SET_STATE_received);
                  NEW_SWITCH_STATE=((SET_STATE_received&0x08)==0x08)?true:false;
                  Serial.print("New state of switch S3 is ");Serial.println(NEW_SWITCH_STATE);
                  status_update(3,NEW_SWITCH_STATE);
                }
                else
                {
                  Serial.println("More than one switch state has changed! Notify current state to server");
                  send_to_server(0);
                }
              }
              else if(received_commands[i] == "SDST")
              {
                  send_to_server(0);
                  //code to be written
              }
              else if(received_commands[i] == "SWMN")
              {
                  //code to be written
              }  
              else if(received_commands[i] == "SWRM")
              {
                  //code to be written
              }
            }
          }
            else
            {//MASTER_ID matches, but NODE_ID does not match=>retransmit
             Serial.println("NODE ID does not match! Re-transmitting");
              send_data(data_to_send);
            }
        }
          else
          {
            //MASTER_ID does not match, discard payload!
            Serial.println("MASTER ID does not match, drop payload!");
          }
      }
      clear_all();
}
