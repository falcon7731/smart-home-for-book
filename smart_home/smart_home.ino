#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//I2C pins declaration
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
//requiers libraries

// Smart garden pins
int pump_pin_number = 13;
int soil_water_level_pin = 12;
int fan_pin_number = 11
int LM35_pin_number = A0;
int UltraSonic_Echo_pin_number = 10;
int UltraSonic_Trig_pin_number = 9;
//home security pins
int ReedSwitch_pin_number = 8;
int MQ5_pin_number = A1;
int Buzzer_pin_number = 6;
//LCD Display pins
int SDA_pin_number = A4;
int Scl_pin_numberr = A5;
//General variables
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
void setup() {
  //setting up pinmodes
  pinMode(pump_pin_number , OUTPUT);
  pinMode(soil_water_level_pin , INPUT);
  pinMode(fan_pin_number , OUTPUT);
  pinMode(LM35_pin_number , INPUT);
  pinMode(ReedSwitch_pin_number , INPUT);
  pinMode(MQ5_pin_number , INPUT);
  pinMode(Buzzer_pin_number , OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

}
