#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_Si7021.h>
#include <RTClib.h>
#include <HardwareSerial.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  Serial.print(result);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

//this is a test of changes made and then uploaded to git
//this a ANOTHER test to see if i can upload changes from my labtop
//This is a new branch to compare code with another branch 
//GOtta do one more to test the branch and stage process
//This is the last one
//testing branch
//test chsges
//Test again
//More tests
//kkkkkkk