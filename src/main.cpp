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

//important global integers in regards to the time 
uint16_t years       = 2023; 
uint8_t months       = 01; 
uint8_t days         = 01;
uint8_t hours        = 00;
uint8_t minutes      = 00;
uint8_t seconds      = 30;

static float Temperature    = 0.0;
static float Humidity       = 0.0;

int moistureSensor = 0;   //A0 pin for the ADC converter
//const int redLEDPin = 3;           // red LED connected to digital pin (This is the valve in this simulation)
float VoltageTotal;
float SensorAverage;
int x;
float RawValue;


#define Valve_A    16
#define Valve_B    17

RTC_DS3231 RTC;
DateTime now;
Adafruit_Si7021 tempHumid;
uint32_t result;

//initial string inputs
String userInput_1    = "";
String userInput_2    = "";
String yearInput      = "";
String monthInput     = "";
String dayInput       = "";
String hourInput      = "";
String minuteInput    = "";
String secondInput    = "";
const byte ROWS = 4; 
const byte COLS = 4; 


//Time state enums that help us accomodate the states the machine is in

enum TimeState
{
    START,
    INITIALIZE,
    MAIN_MENU,
    WAKE_MODE,
    SLEEP_MODE,
    ENTER_YEAR,
    ENTER_MONTH,
    ENTER_DAY,
    ENTER_HOUR,
    ENTER_MINUTE,
    ENTER_SECOND,
    CONFIRM_INPUT,
    ERROR,
    AUTOMATIC_MODE,
    DONE
};

enum WateringMode{
  ON,
  OFF
};

WateringMode WateringState= OFF;
TimeState currentMenu = START;
//KEYPAD SETUP BEGINS

//our keypad setup
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9,8,7,6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5,4,3,2}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


//END OF KEYPAD SET UP
void adc() {
  //Let's average some values together to get a more accurate reading
  Serial.print("Average of 50:\n");
//Will need to add an if statement to check if its nighttime, so that it only runs at night

  RawValue=0; //Have to reset the values before the for loop because they will add onto the previous iteration and youll get a value >5V which is bad
  SensorAverage=0;
  VoltageTotal=0;
  for (x=0; x<50; x++){
    RawValue=analogRead(moistureSensor);
    SensorAverage= RawValue*0.00488;//Use this conversion since ADC has 1024 bits of resolution
    Serial.println(SensorAverage);
    VoltageTotal=VoltageTotal + SensorAverage;
    delay(500); //Will take a reading every 1 second
    //Will add a way to kick out any outliers
    }
  
  SensorAverage=VoltageTotal/50;//To find average
  Serial.print("Sensor Average:");//checking serial port
  Serial.println(SensorAverage);//Checked the values in excel, its averaging them correctly

  //Now we will use this averaged value to determine when to open or close the valves
    if (SensorAverage <= 2.0)  {
    digitalWrite(Valve_A, HIGH);      // if moisture is less than 300 (dry soil) turn the valve on
    Serial.print("High Loop\n");
    //delay(900000) //This will allow it to water for 15 minutes will comment in once ready for testing
    }
    if (SensorAverage > 2.0) {
      digitalWrite(Valve_A,LOW);  //We will have to reset it otherwise the valve will stay on
      //delay(900000) //This will close the valves for 15 minutes and then recheck the moisture level
    }
}

void printRealTime(){
  Serial.print("Date: ");
  Serial.print(now.month());
  Serial.print(" ");
  Serial.print(now.day());
  Serial.print(", ");
  Serial.print(now.year());

  Serial.print(" Time: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.print(" ");
};

void printTempHumid(){
    Temperature= tempHumid.readTemperature();
    Humidity= tempHumid.readHumidity();

    Serial.print("Temperature: ");
    Serial.print(Temperature);
    Serial.print("°C ");
    Serial.print("Humidity: ");
    Serial.print(Humidity);
    Serial.println("%");

}

void initRTC(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t min, uint8_t s){
  RTC.begin();
  RTC.adjust(DateTime(y, m, d, h, min, s));
  
};

void loopRTC(){
  
  while (1){
    delay(1000);
    
    now = RTC.now();
    
    printRealTime();
    printTempHumid();
    
    /*if (now.second()%10 == 0 || now.second()%10 == 0){
      
      switch(WateringState){
        case OFF:
          Serial.println("Watering was off, turning on...");
          WateringState=ON;
          digitalWrite(Valve_A, HIGH);
          digitalWrite(Valve_B, HIGH);
        break;
        case ON:
          Serial.println("Watering was on, turning off...");
          WateringState=OFF;
          digitalWrite(Valve_A, LOW);
          digitalWrite(Valve_B, LOW);
        break;
      }*/
    adc();
  }
}
// A function that returns the number of characters in the string object
int getLength(const String& s)
{
    // Initialize the counter
    int count = 0;
    
    // Use a range-based for loop to iterate over the string object
    for (char c : s)
    {
        // Increment the counter
        count++;
    }
    
    // Return the counter
    return count;
}
// A function that converts a string to an int
// Returns 0 if the string is empty or invalid
int stringToInt(const String& s)
{
    // Initialize the result and the sign
    int result = 0;
    int sign = 1;
    
    // Iterate over the string from the beginning
    
    for (int i = 0; i < getLength(s); i++)
    {
        // Get the current character
        char c = s[i];
        
        // If the first character is a minus sign, set the sign to -1
        if (i == 0 && c == '-')
        {
            sign = -1;
        }
        // Else if the character is a digit, add it to the result
        else if (c >= '0' && c <= '9')
        {
            result = result * 10 + (c - '0');
        }
        // Else if the character is not a digit, return 0
        else
        {
            return 0;
        }
    }
    
    // Return the result with the sign
    return result * sign;
}

//Prints to LCD
void printToLCD(int line, String message)
{
    lcd.setCursor(0, line);
    lcd.print(message);
}
//displaying to the menu
void displayMainMenu(TimeState state)
{

    switch (state)
    {
        case START:
        break;

        case INITIALIZE:
            lcd.clear();
            printToLCD(0, "Welcome to Smart ");
            printToLCD(1, "Drip Irrigation!");
            printToLCD(2, "Lets set the time");
            printToLCD(3, "Press key to start!");
        break;

        case MAIN_MENU:
            lcd.clear();
            printToLCD(0, "1: Automatic");
            printToLCD(1, "2: Selectable");
        break;
        
        case ENTER_YEAR:
            lcd.clear();
            printToLCD(0, "Input current year");
            printToLCD(1, "in 4 digit format:");
            printToLCD(3, "Press # to continue");
        break;

        case ENTER_MONTH:
            lcd.clear();
            printToLCD(0, "Input current month");
            printToLCD(1, "in 2 digit format:");
            printToLCD(3, "Press # to continue");
        break;

        case ENTER_DAY:
            lcd.clear();
            printToLCD(0, "Input current day");
            printToLCD(1, "in 2 digit format:");
            printToLCD(3, "Press # to continue");
        break;

        case ENTER_HOUR:
            lcd.clear();
            printToLCD(0, "Input current hour");
            printToLCD(1, "in 2 digit format:");
            printToLCD(3, "Press # to continue");
        break;

        case ENTER_MINUTE:
            lcd.clear();
            printToLCD(0, "Input current minute");
            printToLCD(1, "in 2 digit format:");
            printToLCD(3, "Press # to continue");
        break;

        case ENTER_SECOND:
            lcd.clear();
            printToLCD(0, "Input current second");
            printToLCD(1, "in 2 digit format:");
            printToLCD(3, "Press # to continue");
        break;

        case CONFIRM_INPUT:
            lcd.clear();
            printToLCD(0, "Confirm your entered");
            printToLCD(1, "date and time ");
            printToLCD(3, "Press # to continue");
        break;

        case ERROR:
            lcd.clear();
            printToLCD(0, "ERROR: Invalid Input");
            printToLCD(1, "     Try again!");
            printToLCD(3, "Press # to continue");
        break;

        case AUTOMATIC_MODE:
            lcd.clear();
            printToLCD(0, "Entering automatic");
            printToLCD(1, "mode...");
          break;

        default:
        break;
    }
}
//handling menu inputs
void handleMenuInput(char key)
{
    switch (currentMenu)
    {
        case START:
            currentMenu = INITIALIZE;
            displayMainMenu(currentMenu);
        break;

        case INITIALIZE:
            currentMenu = ENTER_YEAR;
            displayMainMenu(currentMenu);
        break;

        case ENTER_YEAR:
            lcd.setCursor(0,2);
            yearInput = ""; // Clear yearInput before entering a new year

            while (true)
            {
                if (key >= '0' && key <= '9' && yearInput.length() < 4)
                {
                    yearInput += key;
                    lcd.print(key); // Print the digit as it's entered
                }
                else if (key == '#')
                {
                    break; // Exit the loop when '#' is entered
                }

                key = customKeypad.getKey();
            }

            years = stringToInt(yearInput);

            if (years < 2000 || years > 2099)
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Year");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
                
            }
            else
            {
                currentMenu = ENTER_MONTH;
                displayMainMenu(currentMenu);
            }
        break;

        case ENTER_MONTH:
            lcd.setCursor(0,2);
            monthInput = ""; // Clear yearInput before entering a new year

            while (true)
            {
                if (key >= '0' && key <= '9' && monthInput.length() < 2)
                {
                    monthInput += key;
                    lcd.print(key); // Print the digit as it's entered
                }
                else if (key == '#')
                {
                    break; // Exit the loop when '#' is entered
                }

                key = customKeypad.getKey();
            }

            months = stringToInt(monthInput);

            if (months < 1 || months > 12)
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Month");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            else
            {
                currentMenu = ENTER_DAY;
                displayMainMenu(currentMenu);
            }
        break;

        case ENTER_DAY:
            lcd.setCursor(0,2);
            dayInput = ""; // Clear yearInput before entering a new year
            
            while (true)
            {
                if (key >= '0' && key <= '9' && dayInput.length() < 2)
                {
                    dayInput += key;
                    lcd.print(key); // Print the digit as it's entered
                }
                else if (key == '#')
                {
                    break; // Exit the loop when '#' is entered
                }

                key = customKeypad.getKey();
            }

            days = stringToInt(dayInput);
            if (months == 1 && (days < 1 || days > 31)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            // Why will 2024 be a leap year? To check if a year is a leap year, it must be divisible by 4 and not divisible by 100 or divisible by 400.
            if (months == 2 && (days < 1 || days > 29 ) && years == 2024) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 2 && (days < 1 || days > 28)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 3 && (days < 1 || days > 31)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 4 && (days < 1 || days > 30)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 5 && (days < 1 || days > 31)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 6 && (days < 1 || days > 30)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 7 && (days < 1 || days > 31)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 8 && (days < 1 || days > 31)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 9 && (days < 1 || days > 30)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 10 && (days < 1 || days > 31)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 11 && (days < 1 || days > 30)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            if (months == 12 && (days < 1 || days > 31)) 
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Day");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            else
            {
                currentMenu = ENTER_HOUR;
                displayMainMenu(currentMenu);
            }
        break;

        case ENTER_HOUR:
            lcd.setCursor(0,2);
            hourInput = ""; // Clear yearInput before entering a new year
            
            while (true)
            {            
                if (key >= '0' && key <= '9' && hourInput.length() < 2)
                {
                    hourInput += key;
                    lcd.print(key); // Print the digit as it's entered
                }
                else if (key == '#')
                {
                    break; // Exit the loop when '#' is entered
                }

                key = customKeypad.getKey();
            }

            hours = stringToInt(hourInput);

            if (hours < 0 || hours > 24)
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Hour");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            else
            {
                currentMenu = ENTER_MINUTE;
                displayMainMenu(currentMenu);
            }
        break;

        case ENTER_MINUTE:
            lcd.setCursor(0,2);
            minuteInput = ""; // Clear yearInput before entering a new year

            while (true)
            {
                if (key >= '0' && key <= '9' && minuteInput.length() < 2)
                {
                    minuteInput += key;
                    lcd.print(key); // Print the digit as it's entered
                }
                else if (key == '#')
                {
                    break; // Exit the loop when '#' is entered
                }

                key = customKeypad.getKey();
            }

            minutes = stringToInt(minuteInput);

            if (minutes < 0 || minutes > 60)
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Mins");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            }
            else
            {
                currentMenu = ENTER_SECOND;
                displayMainMenu(currentMenu);
            }
        break;

        case ENTER_SECOND:
            lcd.setCursor(0,2);
            secondInput = ""; // Clear yearInput before entering a new year

            while (true)
            {
                if (key >= '0' && key <= '9' && secondInput.length() < 2)
                {
                    secondInput += key;
                    lcd.print(key); // Print the digit as it's entered
                }
                else if (key == '#')
                {
    
                    break; // Exit the loop when '#' is entered
                }
                
                key = customKeypad.getKey();
            }

            seconds = stringToInt(secondInput);

            if (seconds < 0 || seconds > 60)
            {
                displayMainMenu(currentMenu);
                lcd.clear();
                printToLCD(0, "ERROR: Invalid Secs");
                printToLCD(1, "   Enter again:");
                printToLCD(3, "Press # to continue");
            } 
            else
            {
                currentMenu = CONFIRM_INPUT;
                displayMainMenu(currentMenu);
            }
        break;

        case CONFIRM_INPUT:            
            while (true)
            {
                if (key == '#')
                {
                    lcd.clear();
                    printToLCD(0, "Your date & time: ");
                    printToLCD(1, yearInput + '/' + monthInput + '/' + dayInput + "-" + hourInput + ':' + minuteInput + ':' + secondInput);
                    printToLCD(3, "Press # for Menu");


                    while (true)
                    {
                        key = customKeypad.getKey();
                        
                        if (key == '#')
                        {
                            //RTC.adjust(DateTime(years, months, days, hours, minutes, seconds));
                            initRTC(years, months, days, hours, minutes, seconds);
                            currentMenu = MAIN_MENU;
                            displayMainMenu(currentMenu);
                            break;
                        }
                    }
                    break;
                }

                else
                {
                    currentMenu = CONFIRM_INPUT;
                    displayMainMenu(currentMenu);
                    break;
                }        

                key = customKeypad.getKey();
            }
            
        break;

        case MAIN_MENU:
        while (true){
            if(key == '1'){
              currentMenu= AUTOMATIC_MODE;
              displayMainMenu(currentMenu);
              break;
            }
            else if(key =='2'){
              currentMenu= MAIN_MENU;
              displayMainMenu(currentMenu);
              break;
            }
            key = customKeypad.getKey();
        } 
        break;

        case AUTOMATIC_MODE:  
        while(true){
          loopRTC();

        }
        break;

        case ERROR:
            while (true)
            {
                if (key == '#')
                {
                    currentMenu = MAIN_MENU;
                    displayMainMenu(currentMenu);                   
                    break;
                }

                key = customKeypad.getKey();
            }               
        break;

        default:
        break;
    }
}
//set up
void setup(){
  Serial.begin(115200); //KEEP THIS NUMBER it starts the correct serial port
  pinMode(PC2, OUTPUT);
  pinMode(PC3, OUTPUT);
  TimeState currentMenu = START;
  lcd.init();          // initialize the lcd 
  lcd.backlight();
  lcd.clear();

  Wire.begin(); //starts i2c interface

  lcd.clear();
  // printToLCD(0, "Welcome to Smart ");
  // printToLCD(1, "Drip Irrigation!");
  // printToLCD(2, "Lets set the time");
  // printToLCD(3, "Press # to start!");
  lcd.clear();
  // printToLCD(0, "Set time");

}

void loop ()
{
    char key = customKeypad.getKey(); 
    
    if (key ) 
    {
        handleMenuInput(key); 
        Serial.println(key);
    }
}