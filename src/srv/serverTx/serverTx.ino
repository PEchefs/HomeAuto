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
String stringToSend = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup(void)
{
  Serial.begin(9600);
	Serial.println("ROLE:Transmitter");
	stringToSend.reserve(200);

	if (!rf22.init())
        Serial.println("RF22 init failed");
        // TODO : Check: Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36    
 }

void loop(void)
{
  if (stringComplete) {
	Serial.print("Sending ");
	Serial.println(stringToSend);
 
// TODO: Following is a hack to convert the received string to an unsigned character array. Need to change this. 
	int stringToSendLength = stringToSend.length() + 1; 
	unsigned char* val = (unsigned char*) stringToSend.c_str();
    rf22.send(val, sizeof(val));
    rf22.waitPacketSent();

// TODO: Check success of sending via RF

	Serial.println("Sending ok");
	stringToSend = "";
//TODO: Clear val
//	val[]="";
  stringComplete = false;
}
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    stringToSend += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}
