# LORA-Weather-Balloon
Code for sending weather balloon gps coordinates using Adafruit RFM95W LORA breakouts and the Sparkfun GP-20U7 GPS receiver.

## Flashing code to Arduino
In the two folders `Transmitter` and `Receiver` there are `-LORA_GPS.ino` files, they should be uploaded to the transmitter and receiver arduino.

## Wiring
Both arduinos should be wired to the Adafruit RFM95W LORA breakouts using `Digital 4` as `CS`, `Digital 2` as `RST`, and `Digital 3` as `INT`, along with 5v and ground however is desired.

As for the Sparkfun GP-20U7 GPS receiver, the `RX` out on the breakout should be connected to `Digital 5` on the Arduino. `TX` is not needed.

## Troubleshooting 

If you don't see `LAT =` and `LON =` and instead `"No new data try going outside"` the GPS receiver mostly likely hasn't locked onto a satalite. Try moving a more open area to gain connection, it may take a while.

If the LORA boards don't seem to be initializing check if you changed the frequency at line 22 `#define RF95_FREQ 915.0` to the frequency of the boards you bought. It should be on the product page.

