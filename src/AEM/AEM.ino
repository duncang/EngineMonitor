

#include <CAN.h>
#include <SPI.h>


#define BUS_SPEED 100

#define FUEL_LL_PIN  3

int pin;
int fuel_state;

void setup()
{

  Serial.begin(9600);

  // status LEDs
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  
  // fuel sensor
  pinMode(FUEL_LL_PIN,INPUT);
  

  // initialize CAN bus class
  // this class initializes SPI communications with MCP2515
  CAN.begin();
  CAN.baudConfig(BUS_SPEED);
  CAN.setMode(LOOPBACK);  // set to "NORMAL" for standard com
  
  pin = LOW;  
  
}

void loop()
{

    
  //signal iterations on pin 8
  digitalWrite(8,pin);  
  
  // read fuel sensor status - HIGH = FUEL LOW; LOW = FUEL PRESENT
  fuel_state = digitalRead(FUEL_LL_PIN);
  
  //digitalWrite(7,(fuel_state == 1) ? HIGH : LOW);
  digitalWrite(7,fuel_state);
  
  
  Serial.print("Fuel: ");
  Serial.print(fuel_state,DEC);
  Serial.println();
  
  
    
  if (pin == LOW) {
    pin = HIGH;
  } else {
    pin = LOW;
  }  
  
  
  delay(500);
  
}


