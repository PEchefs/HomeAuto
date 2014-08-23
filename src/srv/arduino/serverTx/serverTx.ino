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
char rec_val_new[32];


void setup(void)
{
  Serial.begin(9600);
	Serial.println("ROLE:Transmitter");
	//stringToSend.reserve(200);

	if (!rf22.init())
        Serial.println("RF22 init failed");
        // TODO : Check: Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36    
 }

void loop(void)
{
	// Trasmission part
  if (stringComplete) {
	Serial.print("Sending ");
//	Serial.println(stringToSend);
 
// TODO: Following is a hack to convert the received string to an unsigned character array. Need to change this. 
	//int stringToSendLength = stringToSend.length() + 1; 
	//uint8_t* val = (uint8_t*) stringToSend.c_str();
    rf22.send(stringToSend, sizeof(stringToSend));
    rf22.waitPacketSent();

// TODO: Check success of sending via RF

	Serial.println("Sending ok");
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
		if(data_to_be_processed=true)
		{
			Serial.println("Received data. Sending to Pi via UART...");
			sendToPiUART();
		}
	  }
  
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

void sendToPiUART()
{
	Serial.print("Sending data: ");
	for(unsigned short int index=0;index<32;index++)
      {
       Serial.print(rec_val_new[index]);
       Serial.write(rec_val_new[index]);
      }
}
