// rf22_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RF22 class. RF22 class does not provide for addressing or reliability.
// It is designed to work with the other example rf22_server

#include <SPI.h>
#include <RF22.h>

// Singleton instance of the radio
RF22 rf22;

void setup() 
{
  Serial.begin(9600);
//  rf22.setTxPower(RF22_TXPOW_17DBM);
  rf22.setModemConfig(RF22::GFSK_Rb2_4Fd36);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  Serial.println("ROLE: CLIENT");
}

void loop()
{
  while (1)
  {
    uint8_t i[1]={0};
    Serial.println("Sending to rf22_server");
    // Send a message to rf22_server
    ///uint8_t data[] = "Hello World!";
    ///rf22.send(data, sizeof(data));
   for(int j=0;;j++)
   {
     Serial.print("Sending: ");
     Serial.println(i[0]);
     rf22.send(i,sizeof(i));
     rf22.waitPacketSent();
    // Now wait for a reply
    uint8_t buf[RF22_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf22.waitAvailableTimeout(500))
    { 
      // Should be a message for us now   
      if (rf22.recv(buf, &len))
      {
        Serial.print("got reply: ");
        Serial.println(buf[0]);
      }
      else
      {
        Serial.println("recv failed");
      }
    }
    else
    {
      Serial.println("No reply, is rf22_server running?");
    }
     i[0]++;
     delay(1000);
   }
  }
}

