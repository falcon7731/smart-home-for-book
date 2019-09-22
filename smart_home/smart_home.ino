//requiers libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
// Smart garden pins
int pump_pin_number = 13;
int soil_water_level_pin = A3;
int fan_pin_number = 11;
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
LiquidCrystal_I2C lcd(0x27, 16, 2);
float Temperature_C = 0.00;
int soil_water_level = 0;
bool is_soil_dry = false;
int fanSpeed = 0;
int fanLCD = 0;
float water_level = 0;
float water_volume = 0;
//controll variables
int thresholdValue = 800;
int max_temprature = 70;
int min_temprature = 30;
int water_tank_radios = 5;
int water_tank_max_height = 50;




void setup() {
  //setting up pinmodes
  pinMode(pump_pin_number , OUTPUT);
  pinMode(soil_water_level_pin , INPUT);
  pinMode(fan_pin_number , OUTPUT);
  pinMode(LM35_pin_number , INPUT);
  pinMode(ReedSwitch_pin_number , INPUT);
  pinMode(MQ5_pin_number , INPUT);
  pinMode(Buzzer_pin_number , OUTPUT);
  pinMode(UltraSonic_Trig_pin_number, OUTPUT);
  pinMode(UltraSonic_Echo_pin_number, INPUT);
  //starting the lcd
  lcd.begin(); //Defining 16 columns and 2 rows of lcd display
  lcd.backlight();//To Power ON the back light
  //starting serial com
  Serial.begin(9600);

}

void loop() {
  //gathering all the data
  Get_temprature();
  Get_soil_status();
  //controlling the envirment
  Fan_Controll();


}

void Get_temprature() {
  int val = analogRead(LM35_pin_number);
  float mv = ( val / 1024.0) * 5000;
  float cel = mv / 10;
  Serial.print("TEMPRATURE = ");
  Serial.print(cel);
  Serial.print("*C");
  Serial.println();
  Temperature_C = cel;
}

void Get_soil_status() {
  int soil_water_level = analogRead(soil_water_level_pin);
  Serial.print(soil_water_level);
  if (soil_water_level < thresholdValue) {
    Serial.println(" - Doesn't need watering");
    is_soil_dry = false;
  }
  else {
    Serial.println(" - Time to water your plant");
    is_soil_dry = true;
  }
}


void Fan_Controll() {
  if (Temperature_C  < min_temprature) { // if temp is lower than minimum temp
    fanSpeed = 0; // fan is not spinning
    digitalWrite(fan_pin_number, LOW);
  }
  if (Temperature_C  > max_temprature) { // if temp is higher than maximum temp
    fanSpeed = 255; // fan is spinning
    digitalWrite(fan_pin_number, HIGH);
  }
  if ((Temperature_C  >= min_temprature) && (Temperature_C <= max_temprature)) { // if temperature is higher than minimum temp
    fanSpeed = map(Temperature_C, min_temprature, max_temprature, 32, 255); // the actual speed of fan
    fanLCD = map(Temperature_C, min_temprature, max_temprature, 0, 100); // speed of fan to display on LCD
    analogWrite(fan_pin_number, fanSpeed); // spin the fan at the fanSpeed speed
  }
}



void Get_water_level() {
  long t = 0, h = 0, hp = 0;

  // Transmitting pulse
  digitalWrite(UltraSonic_Trig_pin_number, LOW);
  delayMicroseconds(2);
  digitalWrite(UltraSonic_Trig_pin_number, HIGH);
  delayMicroseconds(10);
  digitalWrite(UltraSonic_Trig_pin_number, LOW);
  // Waiting for pulse
  long duration = pulseIn(UltraSonic_Echo_pin_number, HIGH);
  // Calculating distance
  int distance = duration * 0.034 / 2;
  //calculating water height
  water_level = water_tank_max_height - distance;
  water_volume = M_PI* pow(water_tank_radios,2)*water_level;
}
