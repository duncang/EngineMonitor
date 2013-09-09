

//#include <CAN.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

int thermoDO = 50;
int thermoCS = 49;
int thermoCLK = 52;

Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);

#define BUS_SPEED 100

#define FUEL_LL_PIN  3


/* Attach hall sensor to interrupt 5 - 
   See http://arduino.cc/en/Main/ArduinoBoardMega2560 
   and http://arduino.cc/en/Reference/AttachInterrupt 
 */
#define RPM_INT 0 
#define RPM_PIN 2  

int pin;
int fuel_state;

volatile unsigned long rpm_count;
unsigned long last_time;
unsigned short rpm;

void setup()
{

  Serial.begin(9600);

  // status LEDs
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  
  // fuel sensor
  pinMode(FUEL_LL_PIN,INPUT);
  
  // RPM sensor
  attachInterrupt(RPM_INT,rpm_tick,RISING);
  rpm_count = 0;
  last_time = millis();

  // initialize CAN bus class
  // this class initializes SPI communications with MCP2515
//  CAN.begin();
//  CAN.baudConfig(BUS_SPEED);
//  CAN.setMode(LOOPBACK);  // set to "NORMAL" for standard com
  
  pin = LOW;  
  
  delay(500);
    
  
}

void loop()
{

    
  //signal iterations on pin 8
  digitalWrite(8,pin);  
  
  // read fuel sensor status - HIGH = FUEL LOW; LOW = FUEL PRESENT
  fuel_state = digitalRead(FUEL_LL_PIN);
  
  //digitalWrite(7,(fuel_state == 1) ? HIGH : LOW);
  digitalWrite(7,fuel_state);
  
  // update RPM
  if (rpm_count > 20)
  {
    
     unsigned long time_now = millis();
     unsigned long time_since = time_now - last_time;
     
     rpm = (60000 * rpm_count)/time_since ;
    
     last_time = time_now;
     rpm_count = 0;   
  }
  
//   Serial.print("Internal Temp = ");
//   Serial.println(thermocouple.readInternal());


  
   double cht = thermocouple.readCelsius();
   if (isnan(cht)) {
      cht = 0.;
   }
   
   
  Serial.print("Fuel: ");
  Serial.print(fuel_state,DEC);
  Serial.print(" RPM: ");
  Serial.print(rpm, DEC);
  Serial.print(" CHT: ");
  Serial.print(cht, DEC);
  Serial.println();
  
  
    
  if (pin == LOW) {
    pin = HIGH;
  } else {
    pin = LOW;
  }  
  
  
  delay(500);
  
}

void rpm_tick()
{
  ++rpm_count;
}




