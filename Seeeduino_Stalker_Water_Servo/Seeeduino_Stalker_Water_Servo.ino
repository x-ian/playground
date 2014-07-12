// Seeeduino Stalker 2.2
// Use board Arduino Pro or Pro Mini (3.3V, 8MHz) w/ ATmega 328
// http://www.seeedstudio.com/wiki/Seeeduino_Stalker_v2.2

// Open & close a value with a servo (attached to PIN 9) for a once-per-day watering activity
// Use Seeeduino Stalker with Arduino Pro or Pro Mini (3.3V, 8 MHz) w/ ATmega328

#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h> 
#include <Servo.h> 
#include <EEPROM.h>

Servo servo; 
byte restartCount = 0;
const int restartCountAdr = 0;
byte previousDayRun = 0; // start counting days from 7.7.2014 onwards (something like 1404769196 seconds)
const int previousDayRunAdr = 1;

const int openValueForMs = 60000;

void setup() 
{ 
  Serial.begin(9600);
  //oneTimeEepromInit();

  servo.attach(9);  // attaches the servo on pin 9 to the servo object 
  closeValve();

  delay(10000);

  eepromRead();  
  Serial.print("Restart count is: ");
  Serial.println(restartCount);
  restartCount++;
  eepromWrite();
  
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  }
  Serial.print("Current time is ");
  digitalClockDisplay();
  Serial.print("seconds since 1970: ");
  Serial.println(now());

  Serial.print("Last successful day of watering; ");
  Serial.println(previousDayRun);
  long currentDayRun = (now() / 60 / 60 / 24) - 16257;
  Serial.print("Current day: ");
  Serial.println(currentDayRun);
  
  delay(10000);
  Serial.println("setup done");
} 

void loop() {
  Serial.println("loop");
  if (!alreadyWateredToday() && duringWateringHours()) {
    Serial.println("Open and close valve");
    openValve();
    delay(openValueForMs);
    closeValve();
    //markWateredToday();
  }
  delay(14400000);
  // wait for 4 hours
}

boolean alreadyWateredToday() {
  int currentDayRun = ((now() / 60 / 60 / 24) - 16257);
  Serial.print("Today's day count: ");
  Serial.println(currentDayRun);
  Serial.print("Previous's day count: ");
  Serial.println(previousDayRun);
  return (previousDayRun >= currentDayRun);
}

void markWateredToday() {
  previousDayRun = (now() / 60 / 60 / 24) - 16257;
  EEPROM.write(previousDayRunAdr, previousDayRun);
}

boolean duringWateringHours() {
  Serial.println("Check if during watering hours");
  int h = hour();
  return (h >=8 && h <= 22);
}

void closeValve() {
    servo.write(180);
    delay(5000);
    servo.write(180);
    delay(5000);
}

void openValve() {
    servo.write(0);
    delay(5000);
}

void oneTimeEepromInit() {
  // make sure this is only invoked once!
  EEPROM.write(restartCountAdr, 0);
  EEPROM.write(previousDayRunAdr, 0);
}

void eepromRead() {
  restartCount = EEPROM.read(restartCountAdr);
  previousDayRun = EEPROM.read(previousDayRunAdr);
}

void eepromWrite() {
  EEPROM.write(restartCountAdr, restartCount);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
