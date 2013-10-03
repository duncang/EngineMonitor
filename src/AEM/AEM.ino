

//#include <CAN.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

int thermoDO = 50;
int thermoCS = 49;
int thermoCLK = 52;

Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);

#define BUS_SPEED 100

#define FUEL_LL_PIN  3

#define IGN_BATT_PIN 0 // analog pin 0


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

unsigned long ign_batt_mV;

void setup()
{

  Serial.begin(9600);
  Serial1.begin(57600);

  // status LEDs
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  
  // fuel sensor
  pinMode(FUEL_LL_PIN,INPUT);
  
  // RPM sensor
  attachInterrupt(RPM_INT,rpm_tick,RISING);
  rpm_count = 0;
  last_time = millis();
  
  
  // setup ignition battery
  ign_batt_mV = 0;
  analogReference(DEFAULT);

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
  static unsigned long loopCount=0;
  ++loopCount;
    
  //signal iterations on pin 8
  digitalWrite(8,pin);  
  
  // read fuel sensor status - HIGH = FUEL LOW; LOW = FUEL PRESENT
  fuel_state = digitalRead(FUEL_LL_PIN);
  
  digitalWrite(7,fuel_state);
  
  // update RPM
  unsigned long time_now = millis();
  unsigned long time_since = time_now - last_time;
  rpm = (60000 * rpm_count)/time_since ;
    
  last_time = time_now;
  rpm_count = 0;


  // check if sensor functioning
  double internal = thermocouple.readInternal();
  if (isnan (internal) || internal == 0.)
  {
     Serial.print("### THERMOCOUPLE FAULT - Internal ####"); 
  }
  
   double cht = thermocouple.readCelsius();
   if (isnan(cht)) {
      cht = 0.;
      Serial.print("### THERMOCOUPLE FAULT - External ####"); 
   }
   
   
   // read ignition battery
   ign_batt_mV = analogRead(IGN_BATT_PIN);
   ign_batt_mV = (ign_batt_mV * 1204) / 100;  // TODO: fix scale factor
   
  Serial.print(time_now,DEC);
  Serial.print(" Fuel: ");
  Serial.print(fuel_state,DEC);
  Serial.print(" RPM: ");
  Serial.print(rpm, DEC);
  Serial.print(" CHT: ");
  Serial.print(cht, DEC);
  Serial.print(" IGN BATT: ");
  Serial.print(ign_batt_mV,DEC);
  
  Serial.println();
  
  // transmit on radio modem
  Serial1.print(time_now,DEC);
  Serial1.print(" Fuel: ");
  Serial1.print(fuel_state,DEC);
  Serial1.print(" RPM: ");
  Serial1.print(rpm, DEC);
  Serial1.print(" CHT: ");
  Serial1.print(cht, DEC);
  Serial1.print(" IGN BATT: ");
  Serial1.print(ign_batt_mV,DEC);
  
  Serial1.println();
  
  
    
  if (pin == LOW) {
    pin = HIGH;
  } else {
    pin = LOW;
  }  
  
  
  delay(500);
  
}

void rpm_tick()
{
  // debounce
  static unsigned long last;
  unsigned long now = millis();
  if (now - last > 1)
  {
    ++rpm_count;
  }  
  
  last = now;
  
  //Serial.print(".");
}




