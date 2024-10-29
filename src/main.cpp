#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_Si7021.h>
#include <RTClib.h>
#include <HardwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
//#include <DS3231.h>

using namespace std;

//set up
void setup(){
    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);
    pinMode(13, OUTPUT);

    delay (2000);
}

void loop ()
{
    digitalWrite(0,HIGH);
    digitalWrite(1,HIGH);
    digitalWrite(13,HIGH);
    delay(500);
    digitalWrite(0,LOW);
    digitalWrite(1,LOW);
    digitalWrite(12,LOW);
    delay(500);
}