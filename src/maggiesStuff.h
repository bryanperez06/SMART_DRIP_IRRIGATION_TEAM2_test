//VALVE A (10 CM SENSORS)
int MoistureSensor_A = 0;    //A0 pin for the ADC converter
const int RelaySensor_A=4;   //D4 pin for sensor relay 10 cm
const int redLEDPin = 2;     //red LED connected to digital pin (This is the valve in this simulation)
float VoltageTotal_A;
float SensorAverage_A;
int x;
float RawValue_A;


//VALVE B (30 CM SENSORS)
int MoistureSensor_B = 1;     //A1 pin for the ADC converter
int RelaySensor_B=5;         //D5 pin for sensor relay 10 cm
const int blueLEDPin = 3;     // blue LED connected to digital pin (This is the valve in this simulation)
float VoltageTotal_B;
float SensorAverage_B;
float RawValue_B;


//SD CARD
#include <SD.h>
const int chipSelect = 10; //Move to cs 8 if this doesn't work
File myFile;


void setup() {
 Serial.begin(9600);           //open serial port get rid of serial if using D0 and D1
 pinMode (redLEDPin,OUTPUT);   //VALVE A
 pinMode (blueLEDPin,OUTPUT);  //VALVE B


 pinMode(RelaySensor_A, OUTPUT); //I tried setting them to analog pins but it didn't work
 pinMode(RelaySensor_B, OUTPUT);


 //Setting up the SD CARD MODULE
 while (!Serial); //Wait for serial monitor to connect
 Serial.print("Initializing SD card...");
 if (!SD.begin(chipSelect)) { //initialize sd card and library
   Serial.println("initialization failed. Things to check:");
   while (true);
 }
 Serial.println("initialization done.");//Once connected
}


void loop() {
//Let's average some values together to get a more accurate reading
 String DataString_A = ""; //make string to assemble data for sd card
 String DataString_B = "";
// Serial.print("Raw Data 50 Values:\n");
//Will need to add an if statement to check if its nighttime, so that it only runs at night


 RawValue_A=0; //Have to reset the values before the for loop because they will add onto the previous iteration and youll get a value >5V which is bad
 SensorAverage_A=0;
 VoltageTotal_A=0;
 RawValue_B=0; //Have to reset the values before the for loop because they will add onto the previous iteration and youll get a value >5V which is bad
 SensorAverage_B=0;
 VoltageTotal_B=0;


//ADDING 2 RELAYS SO ITS NOT RUNNING ALL THE TIME
//Start with 10 cm
//We want to set A2 to high every 3 hours to check soil moisture


//If time is after 6 oclock check soil so set both relay sensor high
 digitalWrite(RelaySensor_A,HIGH); //Allow A sensors to run
 digitalWrite(RelaySensor_B,HIGH); //Allow B sensors to run


//Now we will collect our soil moisture value
 for (x=0; x<50; x++){
   RawValue_A=analogRead(MoistureSensor_A);
   RawValue_B=analogRead(MoistureSensor_B);
   SensorAverage_A= RawValue_A*0.00488;//Use this conversion since ADC has 1024 bits of resolution
   SensorAverage_B= RawValue_B*0.00488;


   DataString_A +=String(SensorAverage_A);
   DataString_A += "\n";
   DataString_B +=String(SensorAverage_B);
   DataString_B += "\n";


   VoltageTotal_A=VoltageTotal_A + SensorAverage_A;
   VoltageTotal_B=VoltageTotal_B + SensorAverage_B;
   delay(500); //Takes a reading every second
   }


 SensorAverage_A=VoltageTotal_A/50;//To find average
 SensorAverage_B=VoltageTotal_B/50;//To find average


 //Once we've collected our soil value we can turn off the sensor relays
 digitalWrite(RelaySensor_A,LOW); //Allow A sensors to run
 digitalWrite(RelaySensor_B,LOW); //Allow B sensors to run


//Print these values to the monitor
 Serial.print("Sensor Average A:");
 Serial.println(SensorAverage_A);
 Serial.print("Sensor Average B:");
 Serial.println(SensorAverage_B);


 //Use four different if statements so that the delays can correspond to each valve
 //Now we will use this averaged value to determine when to open or close the valves


 //OPEN BOTH VALVES
   if (SensorAverage_A <= 2.20 && SensorAverage_B <= 2.40)  {
   digitalWrite(blueLEDPin,HIGH);
   digitalWrite(redLEDPin, HIGH); //Short sensors only water for 15 the longer ones will water for 30
   //CHANGE DELAYS FOR FINAL PRODUCT
   delay(15000);//There will need to be a 15 minute delay and then valve A will close
   digitalWrite(redLEDPin,LOW); //close VALVE A
   delay(15000);//After 30 minutes VALVE B will close
   digitalWrite(blueLEDPin,LOW);
   }


 //OPEN VALVE A ONLY (10 cm sensors)
   if (SensorAverage_A <= 2.20 && SensorAverage_B > 2.40)  {
   digitalWrite(blueLEDPin,LOW);
   digitalWrite(redLEDPin, HIGH);
   delay(15000);//Water for 15 minutes
   digitalWrite(redLEDPin,LOW); //close VALVE A
   }


 //OPEN VALVE B ONLY (30 cm sensors)
   if (SensorAverage_A > 2.20 && SensorAverage_B <= 2.40)  {
   digitalWrite(blueLEDPin,HIGH);
   digitalWrite(redLEDPin, LOW);
   delay(30000);//Water for 30 minutes
   digitalWrite(blueLEDPin,LOW); //close VALVE B
   }
 //CLOSE BOTH VALVES
   if (SensorAverage_A > 2.20 && SensorAverage_B > 2.40) {
     digitalWrite(redLEDPin,LOW);
     digitalWrite(blueLEDPin,LOW);


     }


//CREATE THE FILE
 File dataFile = SD.open("datalog.txt", FILE_WRITE);
 if (dataFile) {
   dataFile.println("Soil Moisture_A:\n");
   dataFile.println(DataString_A);
   dataFile.println("Soil Moisture_B:\n");
   dataFile.println(DataString_B);
   dataFile.close();
 }
 // If file doesn't open issue an error
 else {
   Serial.println("error opening datalog.txt");
 }
     //delay(900000) Will recheck after 3 hours
}

