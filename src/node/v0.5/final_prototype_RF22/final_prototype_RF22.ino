/*	PhotoElectricChefs	*/
/*    photoelectricchefs.org	*/

/*  NODE CODE  */
/*  
RF - Arduino Pin Assignments:
----------------
 RF      -  Arduino
----       ---------
 GND     -  GND
 SDN     -  GND
 Vcc     -  Vcc (3v3)
 nIRQ    -  Pin2 
 nSEL    -  Pin10
 SDI     -  Pin11
 SDO     -  Pin12
 SCLK    -  Pin13

Recognized properties and commands(together referred as attributes):
--------------------------------------------------------------------
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
#include <RF22.h>

RF22 rf22;
//Define number of switches controlled by the node

#define N 4
#define SUCCESS 1
#define STATECHANGE 2
#define FAIL 3
#define CURRENTSTATE 4

const unsigned short int switch_pins[N]={8,9,10,11};	//for 4 switch node
const unsigned short int switch_volt_pins[N]={3,4,5,6};

/* Defining starting addresses in EEPROM where each of the switches' state and/or other properties are stored.
   Assuming 20 bytes would suffice for each switch data. Can extend if needed.
   Here addresses 20-39 : SWITCH1, 40-59 : SWITCH2, 60-79 : SWITCH3, 80-99 : SWITCH4 	*/

const unsigned short int switch_addr[N]={20,30,40,50};	//for 4 switch node 
const unsigned short int commom_info_addr=10;           //starting address to store common information such as NODE_ID,MASTER_ID,LAST_COMMAND_ID,LAST_KEEPALIVE_ID,MANUAL_OVERRIDE

/***************************************************/
/************Private Variables**********************/
unsigned char NODE_ID,MASTER_ID,MANUAL_OVERRIDE=N;
unsigned char NODE_ID_received,MASTER_ID_received,SET_STATE_received,COMMAND_ID_received,COMMAND_ID_generated;
bool NEW_SWITCH_STATE;
unsigned char LIVE_STATE,STATE_DIFFERENCE;
//unsigned char LAST_COMMAND_ID,LAST_KEEPALIVE_ID;

///const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };    //pipes for RF communication
uint8_t pipeno[6];
unsigned char data_to_send[35];
//unsigned char data_to_send_tmp[35];
// char data_to_send_tmp[35];
uint8_t data_to_send_tmp[35]="NDID= ,MSID= ,CMID= ,SDST= ,";
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

//Every switch has a pin, state and EEPROM start address associated with it
struct switches
{
unsigned short int pin;
boolean state;						//true=ON, false=OFF
byte addr;
unsigned short int volt_pin;	//voltage sensor input
unsigned short int volt_prev_state;
unsigned short int volt_current_state;
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
unsigned char genCMID();



/***************************************************/
/***********Function Defintions*********************/
void Node_Init()
{	for(i=0;i<N;i++)
	S[i].pin=switch_pins[i];		//
	
	for(i=0;i<N;i++)
	S[i].addr=switch_addr[i];

	for(i=0;i<N;i++)
	pinMode(S[i].pin, OUTPUT);
	
	for(i=0;i<N;i++)
	{
		S[i].volt_pin=switch_volt_pins[i];
		pinMode(S[i].volt_pin, INPUT);
	}

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
           Serial.begin(115200);        
           Serial.println("ROLE:  SLAVE NODE");
           Serial.println("");

      if (!rf22.init())
        Serial.println("RF22 init failed");
        // TODO: Check (probably comment from old code): Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36        
}

void Read_EEPROM()
{
//TEMPORARY ASSIGNMENT FOR NODE_ID AND MASTER_ID
        NODE_ID=0x46;
        MASTER_ID=0x41;
		//TODO: Master ID and Node ID reading is fine. Check where these are assigned and stored in EEPROM.
		//TODO: Also check if it can have clearly defined separate EEPROM addresses instead of common_info_addr
//        NODE_ID=EEPROM.read(commom_info_addr);
//        MASTER_ID=EEPROM.read(commom_info_addr+1);
//        MANUAL_OVERRIDE=EEPROM.read(commom_info_addr+2);
//        LAST_COMMAND_ID=EEPROM.read(commom_info_addr+3);
//        LAST_KEEPALIVE_ID=EEPROM.read(commom_info_addr+4);
	for(i=0;i<N;i++)	
	{
          S[i].state = EEPROM.read(S[i].addr);
	  S[i].volt_prev_state = EEPROM.read(S[i].addr+1);
        }
	//	S[i].id = EEPROM.read(S[i].addr+1); //illustration to read more info abt a particular switch

}



void status_update(unsigned short int switch_number, boolean switch_state)
{
/*Usage Example: status_update(0, true) to turn ON switch S0*/
		 digitalWrite(S[switch_number].pin, (switch_state==true)?HIGH:LOW);	//send control signal to switch ON/OFF a switch
		 S[switch_number].state=switch_state;					//update state in switch's element in structure
		 EEPROM.write(S[switch_number].addr, S[switch_number].state);		//update state in EEPROM
	         Serial.print("DEBUG: S[switch_number].state= ");Serial.println(S[switch_number].state);

		 LIVE_STATE=SET_STATE_received;
		 
		/*Illustration to write another info regarding S1, say ID into EEPROM:
		EEPROM.write(S[switch_number].addr+1, S[switch_number].id); 
		assuming id is an element of switches structure and is defined/passed*/
		
		
}

void receive_data()
{
    unsigned char rec_val[32];
    unsigned char len=sizeof(rec_val);
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
         { // Serial.println("DEBUG:");: Serial.print("Found ',' at rec_val_new[");Serial.print(i);Serial.println("]");Serial.print("Attribute ");Serial.print(k);Serial.print(" : ");Serial.print(j);Serial.print(" to ");
            l=0;
            while(j<i)
               {
                  if(rec_val_new[j]!=0)
                  attribute[k][l]=rec_val_new[j];
                  l++;
                  j++;    
               }
      //     Serial.println("DEBUG:");:    Serial.println(j);
            j++;  //to skip the ','
            k++;  //to store next attribute in next row of attribute[][]
         }
       }
       
   //Separate attribute type and value
   
   for(k=0;k<4;k++)
   {//Serial.println("");
     for(l=0;l<10;l++)
     {// Serial.println("DEBUG:");: Serial.print("k=");Serial.print(k);Serial.print(",l=");Serial.println(l);
       if(attribute[k][l]!=0)
         {
           if(attribute[k][l]=='=')
           {// Serial.println("DEBUG:");: Serial.print("Found '=' at ");Serial.print("k=");Serial.print(k);Serial.print(" l=");Serial.println(l);
             m=0;
             i=0;
             while(m<l)
             {// Serial.println("DEBUG:");: Serial.print("Copying attribute[");Serial.print(k);Serial.print("][");Serial.print(m);Serial.print("] to attr_name[");Serial.print(k);Serial.print("][");Serial.print(i);Serial.println("]");
               attr_name[k][i]=attribute[k][m];
               i++;
               m++;
             }
             m++;
             j=0;
             while(attribute[k][m]!=0)
             {// Serial.println("DEBUG:");: Serial.print("Copying attribute[");Serial.print(k);Serial.print("][");Serial.print(m);Serial.print("] to attr_val[");Serial.print(k);Serial.print("][");Serial.print(j);Serial.println("]");
               attr_val[k][j]=attribute[k][m]-1;
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
       {// Serial.println("DEBUG:"); Serial.print("attr_name[");Serial.print(k);Serial.print("][");Serial.print(l);Serial.print("]= "); 
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
       {//Serial.println("DEBUG:");Serial.print("attr_val[");Serial.print(k);Serial.print("][");Serial.print(m);Serial.print("]= ");
         Serial.print(attr_val[k][l]);
         Serial.print(" ");
 //      Serial.println("DEBUG:");  received_values_list[k]=String(received_values_list[k]+attr_val[k][l]);
       }
     }
     Serial.println();
  // Serial.println("DEBUG:");  Serial.print("received_values_list[");Serial.print(k);Serial.print("] = ");Serial.println(received_values_list[k]);
   }
   
   
 for(i=0,j=0,k=0;received_attributes_list[i]!=0;i++)
 {
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
  //LIVE_STATE=0x00;      // Binary: 00001000 - Switch S3 is ON, S0, S1 and S2 are off.
  return LIVE_STATE;
  
}

void send_to_server(unsigned short int send_code)
{
  /*  send_code mapping:
      1  -  ACK 		 : Send acknowledgement to server (for previous CMID processed)
	  2  -  STATECHANGE  : inform state change (by manual operation) to Node
	  3	 -	FAIL		 : more than one switch state has changed, inform server the present state
	  4  -	CURRENTSTATE : server has requested currrent state, send current state
  */   
  //TODO: Should call send_data to send?
  switch(send_code)
  {
	case 1:	LIVE_STATE=get_state();
			//strncpy((char *)data_to_send_tmp, "NDID= ,MSID= ,CMID= ,ACKL= ,SDST=  ,", sizeof(data_to_send_tmp));
			data_to_send_tmp[5]=NODE_ID+1;
			data_to_send_tmp[12]=MASTER_ID+1;
			data_to_send_tmp[19]=COMMAND_ID_received+1;
			data_to_send_tmp[26]=0x01+1;
			data_to_send_tmp[33]=LIVE_STATE+1;
			break;
    case 2:	LIVE_STATE=get_state();
			COMMAND_ID_generated=genCMID();
		//	strncpy((char *)data_to_send_tmp, "NDID= ,MSID= ,CMID= ,SDST= ,", sizeof(data_to_send_tmp));
           //data_to_send_tmp = "NDID= ,MSID= ,CMID= ,SDST= ";
			data_to_send_tmp[5]=NODE_ID+1;
			data_to_send_tmp[12]=MASTER_ID+1;
			data_to_send_tmp[19]=COMMAND_ID_generated+1;
			data_to_send_tmp[26]=LIVE_STATE+1;
			break;
	case 3: 
	case 4: LIVE_STATE=get_state();
		//	strncpy((char *)data_to_send_tmp, "NDID= ,MSID= ,CMID= ,SDST= ,", sizeof(data_to_send_tmp));
           //data_to_send_tmp = "NDID= ,MSID= ,CMID= ,SDST= ";
			data_to_send_tmp[5]=NODE_ID+1;
			data_to_send_tmp[12]=MASTER_ID+1;
			data_to_send_tmp[19]=COMMAND_ID_received+1;
			data_to_send_tmp[26]=LIVE_STATE+1;
			break;
  }
//send_data(data_to_send_tmp);  
  //send_data(data_to_send_tmp);  
  
  //DEBUG:
   rf22.send(data_to_send_tmp, sizeof(data_to_send_tmp));
      rf22.waitPacketSent();
      Serial.println("Sending ok");
      Serial.println("Sending data: ");
	for(unsigned short int index=0;index<35;index++)
      {
       Serial.print(data_to_send_tmp[index]);
       Serial.print(" ");
      }
      
      
}

/*
//void send_data(unsigned char* data_to_send)

void send_data(uint8_t *data_to_send)
{
   
      rf22.send(data_to_send, sizeof(data_to_send));
      rf22.waitPacketSent();
      Serial.println("Sending ok");
	  //TODO: Verify sending. Not tested
	  //TODO: Add condition to check success of sending
      Serial.println("Sending data: ");
	for(unsigned short int index=0;index<35;index++)
      {
       Serial.print(data_to_send[index]);
       Serial.print(" ");
      }
}
*/

unsigned char genCMID()
{
	return 0xFF;

}
	
void clear_all()
{
  
  //TODO: Function to clear transaction specific data like NODE_ID_received, MASTER_ID_received etc
  NODE_ID_received=0;
  MASTER_ID_received=0;
  SET_STATE_received=0;
  COMMAND_ID_received=0;
  for(i=0;i<33;i++)
  data_to_send[i]=0;
 /* for(i=0;i<33;i++)
  data_to_send_tmp[i]=0;
  */
  data_to_send_tmp[5]=0;
  data_to_send_tmp[12]=0;
  data_to_send_tmp[19]=0;
  data_to_send_tmp[26]=0;
  data_to_send_tmp[33]=0;
  for(i=0;i<32;i++)
   rec_val_new[i]=0;
  for(i=0;i<4;i++)
   for(j=0;j<6;j++)
   for(j=0;j<6;j++)
   attr_name[i][j]=0;
  for(i=0;i<4;i++)
   for(j=0;j<4;j++)
   attr_val[i][j]=0;
    
  for(i=0;i<4;i++)
   for(j=0;j<10;j++)
   attribute[i][j]=0;
  for(i=0;i<5;i++)
  {
  received_attributes_list[i]="";
  received_properties[i]="";
  received_commands[i]="";
  num_commands=0;
  num_properties=0;
  }
}

void resetNode()
{
 //TODO: Function to cater to external reset switch 
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
                Serial.print("DEBUG: LIVE_STATE= ");Serial.println(LIVE_STATE);
                Serial.print("DEBUG: SET_STATE_received= ");Serial.println(SET_STATE_received);
                LIVE_STATE=get_state();
                STATE_DIFFERENCE = LIVE_STATE ^ SET_STATE_received;
                Serial.print("STATE_DIFFERENCE= ");Serial.println(STATE_DIFFERENCE);
                if(STATE_DIFFERENCE == 0x01)
                {
                  NEW_SWITCH_STATE=((SET_STATE_received&0x01)==0x01)?true:false;
                  Serial.print("New state of switch S0 is ");Serial.println(NEW_SWITCH_STATE);
                  status_update(0,NEW_SWITCH_STATE);
		//		  send_to_server(SUCCESS);
                }
                else if(STATE_DIFFERENCE == 0x02)
                {
                  NEW_SWITCH_STATE=((SET_STATE_received&0x02)==0x02)?true:false;
                  Serial.print("New state of switch S1 is ");Serial.println(NEW_SWITCH_STATE);
                  status_update(1,NEW_SWITCH_STATE);
		//		  send_to_server(SUCCESS);
                }
                else if(STATE_DIFFERENCE == 0x04)
                {
                  NEW_SWITCH_STATE=((SET_STATE_received&0x04)==0x04)?true:false;
                  Serial.print("New state of switch S2 is ");Serial.println(NEW_SWITCH_STATE);
                  status_update(2,NEW_SWITCH_STATE);
		//		  send_to_server(SUCCESS);
                }
                else if(STATE_DIFFERENCE == 0x08)
                {
                  Serial.print("SET_STATE_received=");Serial.println(SET_STATE_received);
                  NEW_SWITCH_STATE=((SET_STATE_received&0x08)==0x08)?true:false;
                  Serial.print("New state of switch S3 is ");Serial.println(NEW_SWITCH_STATE);
                  status_update(3,NEW_SWITCH_STATE);
		//		  send_to_server(SUCCESS);
                }
                else
                {
                  Serial.println("More than one switch state has changed! Notify current state to server");
                  // send_to_server(FAIL);
                }
              }
              else if(received_commands[i] == "SDST")
              {
                  send_to_server(CURRENTSTATE);
                  //TODO: code to be written to send live state to server
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
             Serial.println("NODE ID does not match, drop payload!...");
           //   send_data(data_to_send);
          }
        }
          else
          {
            //MASTER_ID does not match, discard payload!
            Serial.println("MASTER ID does not match, drop payload!");
          }
      }
	  else
	  {
		//for(i=0;i<N;i++)	
                // below is only for 2 switch case. This is because the other two pins are floating and detecting them will cause unnecessary state change
                for(i=0;i<2;i++)	
		{
		unsigned long time1=pulseIn(S[i].volt_pin,LOW, 20000);
		//Serial.print(S[i].volt_pin);Serial.print("=");Serial.println(time1);
		if(time1<=1000)
			S[i].volt_current_state=0;
		else
			S[i].volt_current_state=1;
		if(S[i].volt_current_state!=S[i].volt_prev_state)
		{
			if (S[i].state==true)
			{
				SET_STATE_received=(LIVE_STATE^= 1 << i);
				status_update(i,false);
			//	send_to_server(STATECHANGE);
			
			}
			else
			{
				SET_STATE_received=(LIVE_STATE^= 1 << i);
				status_update(i,true);
			//	send_to_server(STATECHANGE);
			}
		}
		S[i].volt_prev_state=S[i].volt_current_state;
		EEPROM.write(S[i].addr+1, S[i].volt_prev_state);		//update state in EEPROM
		}
	}
      clear_all();
}
