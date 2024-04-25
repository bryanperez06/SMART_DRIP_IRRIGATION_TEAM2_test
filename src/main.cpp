#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_Si7021.h>
#include <RTClib.h>
#include <HardwareSerial.h>



RTC_DS3231 RTC;
uint32_t result;
// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  result = myFunction(2, 3);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(result);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

