/******************\
PhotoElectricChefs
/******************\
 RF      -  Arduino
 GND     -  GND
 SDN     -  GND
 Vcc     -  Vcc (3v3)
 nIRQ    -  Pin2 
 nSEL    -  Pin10
 SDI     -  Pin11
 SDO     -  Pin12
 SCLK    -  Pin13
*/

#include <SPI.h>
#include <RF22.h>

RF22 rf22;

// UART receive string
uint8_t stringToSend[32] = {0};         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
bool data_to_be_processed=false;
//char rec_val_new[32];

char rec_val_new[32];   
char sendToPiData[32];
char attribute[4][10];
char attr_name[4][6];
unsigned char attr_val[4][4];
unsigned short int i,j,k,l,m;
String received_attributes_list[5];
String received_properties[5];
String received_commands[5];
//String received_values_list[5];
unsigned short int num_properties,num_commands;

unsigned char NODE_ID_received,MASTER_ID_received,SET_STATE_received,COMMAND_ID_received,COMMAND_ID_generated,NODE_STATE_received;


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
    
   //// Serial.println();
   //// Serial.println("Decoded attribute names:");
    for(k=0;k<4;k++)
   {
     for(l=0;l<6;l++)
     {
       if(attr_name[k][l]!=0)
       {// Serial.println("DEBUG:"); Serial.print("attr_name[");Serial.print(k);Serial.print("][");Serial.print(l);Serial.print("]= "); 
      ////   Serial.print(attr_name[k][l]);
      ////   Serial.print(" ");
         received_attributes_list[k]=received_attributes_list[k]+attr_name[k][l];
       }
     }
  ////   Serial.println();
  ////   Serial.print("[");Serial.print(k);Serial.print("] = ");Serial.println(received_attributes_list[k]);
   }
   
  //// Serial.println();
  ////  Serial.println("Decoded attribute values:");
    for(k=0;k<4;k++)
   {
     for(l=0;l<4;l++)
     {
       if(attr_val[k][l]!=0)
       {//Serial.println("DEBUG:");Serial.print("attr_val[");Serial.print(k);Serial.print("][");Serial.print(m);Serial.print("]= ");
    ////     Serial.print(attr_val[k][l]);
    ////     Serial.print(" ");
 //      Serial.println("DEBUG:");  received_values_list[k]=String(received_values_list[k]+attr_val[k][l]);
       }
     }
 ////    Serial.println();
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
    NODE_STATE_received=attr_val[i][0];
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
////  Serial.print("Number of properties received = ");Serial.println(num_properties);
////  Serial.print("Number of commands received = ");Serial.println(num_commands);
 
  data_to_be_processed=false;
  
}

void clear_all()
{
  
  //TODO: Function to clear transaction specific data like NODE_ID_received, MASTER_ID_received etc
  NODE_ID_received=0;
  MASTER_ID_received=0;
  SET_STATE_received=0;
  COMMAND_ID_received=0;
  NODE_STATE_received=0;
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

void setup(void)
{
  Serial.begin(115200);
	Serial.println("ROLE:Transmitter");
	//stringToSend.reserve(200);

	if (!rf22.init())
        Serial.println("RF22 init failed");
        // TODO : Check: Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36    
 }

void loop(void)
{
	// Trasmission part
//FOR DEBUG:
//stringComplete=true;
  if (stringComplete) {
////	Serial.print("Sending ");
//	Serial.println(stringToSend);
 
// TODO: Following is a hack to convert the received string to an unsigned character array. Need to change this. 
	//int stringToSendLength = stringToSend.length() + 1; 
	//uint8_t* val = (uint8_t*) stringToSend.c_str();

//FOR DEBUG:
/*
stringToSend[0] = 'N';
stringToSend[1] = 'D';
stringToSend[2] = 'I';
stringToSend[3] = 'D';
stringToSend[4] = '=';
stringToSend[5] = 'G';
stringToSend[6] = ',';
stringToSend[7] = 'M';
stringToSend[8] = 'S';
stringToSend[9] = 'I';
stringToSend[10] = 'D';
stringToSend[11] = '=';
stringToSend[12] = 'B';
stringToSend[13] = ',';
stringToSend[14] = 'S';
stringToSend[15] = 'T';
stringToSend[16] = 'S';
stringToSend[17] = 'T';
stringToSend[18] = '=';
stringToSend[19] = 0x02;
stringToSend[20] = ',';
stringToSend[21] = '\n';  
*/


   rf22.send(stringToSend, sizeof(stringToSend));
   rf22.waitPacketSent();

/////DEBUG:
/*Serial.print("DEBUG: Sending data: ");
 for(int i=0;i<32;i++)
   {
     Serial.print(stringToSend[i]);
     Serial.print(" ");
   }
   Serial.println("");
*/

// TODO: Check success of sending via RF

////	Serial.println("Sending ok");
        for(int i=0;i<32;i++)
          stringToSend[i]=0;
//TODO: Clear val
//	val[]="";
  stringComplete = false;
 
}
  
  // Reception part
  
  if (rf22.available())
      {
        receive_data();
        process_received_data();
		if(data_to_be_processed=true)
		{
////			Serial.println("Received data. Sending to Pi via UART...");
      // TODO: Add code to check MasterID
      // Check for valid Node Id and state
			sendToPiUART();
		}
	  }
clear_all();
delay(500);
  
}

void serialEvent() {
  static int i=0;
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inChar == '\n') {
      stringComplete = true;
      i=0;
    } 
    else
    {
      stringToSend[i]= inChar;
      i++;
    }
  }
}

void receive_data()
{
    unsigned char rec_val[32];
    unsigned char len=sizeof(rec_val);
    if (rf22.recv(rec_val, &len))
    {
//      Serial.print("Received data: ");
     /// Serial.println((char*)buf);
      for(unsigned short int index=0;index<32;index++)
      {
//       Serial.print(rec_val[index]);
       rec_val_new[index]=rec_val[index];
//       Serial.print(" ");
      }
////      Serial.println("");
      data_to_be_processed=true;
      delay(20);
    }
////    Serial.println("Done");
 //   Serial.println("");
}

void sendToPiUART()
{
//	Serial.print("Sending data: ");


    
    sendToPiData[0] = NODE_ID_received;
    sendToPiData[1] = NODE_STATE_received;
	for(unsigned short int index=0;index<=1;index++)
      {
 ////      Serial.print(rec_val_new[index]);
   ////    Serial.write(rec_val_new[index]);
   Serial.write(sendToPiData[index]);
      }
        for(int i=0;i<32;i++)
        {
          rec_val_new[i]=0;
        }
 
}
