// LoRa 9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example LoRa9x_RX

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <RH_RF95.h>

//Pins for LORA module
#define RFM95_CS 4
#define RFM95_RST 2
#define RFM95_INT 3

//LORA init code

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);


//GPS init code

/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

//Tiny gps object
TinyGPS gps;

// 5 = rx
// 6 = tx not used but needed to make SoftwareSerial object.
SoftwareSerial ss(5, 6);

//Buffers for storing char array latitude and longitude
char bufferlat[12];
char bufferlon[12];

String sendString;

void setup() 
{
  //LORA setup
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);


  //GPS setup
  ss.begin(9600);
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop()
{
  //GPS read loop
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  uint8_t data_send;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;) {
    while (ss.available()) {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }

  }

  //Only update gps cooridinates if a new valid value is polled
  if (newData) {
    float flat, flon;
    unsigned long age;

    //Getting GPS cooridinates
    gps.f_get_position(&flat, &flon, &age);
    //Checking to see if invalid
    flat = (flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat);
    flon = (flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon);

    Serial.println("LAT=");
    Serial.println(flat, 6); //Prints cooridinates to 6 degrees of persicsion
    Serial.println("LON=");
    Serial.println(flon, 6);

    //Convering to char array
    dtostrf(flat, 11, 6, bufferlat);
    Serial.println(bufferlat);
    dtostrf(flon, 11, 6, bufferlon);
    Serial.println(bufferlon);

    //Print final sting for debugging char array conversion
    Serial.println(String("Sending: ") + String(bufferlat) + String(", ") + String(bufferlon));

    
  } else {
    Serial.println("No new data try going outside"); //Problem typically aireses from no GPS connection

  }

  //Converting to String object, very ew...
  sendString = (String(bufferlat) + String(", ") + String(bufferlon));

  //Empty uint8_t array for conversion
  uint8_t dataArray[sendString.length() + 1];
  
  //Writing final combined Sting object to the empty uint8_t array
  sendString.toCharArray(dataArray, sendString.length() + 1);

  //LORA send Loop
  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  rf95.send(dataArray, sizeof(dataArray));

  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
  
  //Delay between each sent message
  //Decrease for more frequent messages
  delay(1000);

}