#include <SoftwareSerial.h>

#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

TinyGPS gps;
// 5 = rx
// 6 = tx not used but needed to make SoftwareSerial object.
SoftwareSerial ss(5, 6);

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
  
}

void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;) {
    while (ss.available()) {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }

  }

  if (newData) {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    flat = (flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat);
    flon = (flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon);
    Serial.println("LAT=");
    Serial.println(flat, 6);
    Serial.println(" LON=");
    Serial.println(flon, 6);
    
  } else {
    Serial.println("No new data try going outside");

  }

}