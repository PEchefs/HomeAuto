/******************\
PhotoElectricChefs
/******************\
 RF        -  Arduino
 Pin1      -  GND
 Pin2      -  Vcc
 Pin3(CE)  -  Pin7 for our board(acc to arduino pins), 13 on Atmega
 Pin4(CSN) -  Pin8 for our board(acc to arduino pins), 14 on Atmega
 Pin5(SCK) -  SCK
 Pin6(MOSI)-  MOSI
 Pin7(MISO)-  MISO
 Pin8(IRQ) -  No Connection
*/
#define RF22_MAX_MESSAGE_LEN 50


#include <RF22Router.h>
#include <RF22Mesh.h>
#include <SPI.h>
//#include <RF22.h>


#define CLIENT_ADDRESS 2
#define SERVER_ADDRESS 1

///RF22 rf22;
RF22Mesh rf22(SERVER_ADDRESS);
//RF22 rf22;

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 

///RF24 radio(7,8);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup(void)
{
///  Serial.begin(57600);
     Serial.begin(9600);
  Serial.println("ROLE:Transmitter");

  // Setup and configure rf radio

///  radio.begin();
///  radio.setRetries(15,15);
     if (!rf22.init())
        Serial.println("RF22 init failed");
        // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36    
 }

void loop(void)
{
///  radio.openWritingPipe(pipes[0]);
///  radio.openReadingPipe(1,pipes[1]);

 unsigned char val[32] = "NDID=F,MSID=A,CMID= ,STST= ,";
 val[19]=0xF8; //CMID
 val[26]=0x01; //STST
 // unsigned char val=100;
  Serial.print("Sending ");
 /*for(int index=0;index<32;index++)
      {
      Serial.println(val[index]);
      } 
 */     
///  bool ok = radio.write( val, 32*sizeof(unsigned char) );
      //rf22.send(val, sizeof(val));
      if (rf22.sendtoWait(val, sizeof(val), CLIENT_ADDRESS)!= RF22_ROUTER_ERROR_NONE)
        Serial.println("sendtoWait failed");
      //rf22.waitPacketSent();
//    bool ok = radio.write(&val, sizeof(unsigned char));
///  if(ok)
    else
      Serial.println("Sending ok");
///  else
///  Serial.println("sending failed");
  delay(1000);
  while(true);
}

