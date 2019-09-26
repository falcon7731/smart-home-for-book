//requiers libraries
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

// Smart garden pins
int pump_pin_number = 13;
int soil_water_level_pin = A3;
int fan_pin_number = 11;
int DS18B20_pin_number = 4;
int UltraSonic_Echo_pin_number = 10;
int UltraSonic_Trig_pin_number = 9;

//home security pins
int ReedSwitch_pin_number = 8;
int MQ5_pin_number = A1;
int Buzzer_pin_number = 6;
int Lamp_pin_number = 7;
int PIR_pin_number = 5;
int Display_Toggle_Controll_pin_number = 4;

//LCD Display pins
int SDA_pin_number = A4;
int Scl_pin_numberr = A5;

//General variables
LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(DS18B20_pin_number);
DallasTemperature Temprature_sensors(&oneWire);
float Temperature_C = 0.00;
int soil_water_level = 0;
bool is_soil_dry = false;
int fanSpeed = 0;
int fanLCD = 0;
bool pump_state = false;
float water_level = 0;
float water_volume = 0;
int air_quality_value = 0;
bool is_air_breathable = false;
bool Reed_Switch_status = false;
bool is_window_open = false;
bool has_detected_movement = false;
bool was_button_active = false;
int Display_Info_Index = 0;
int Display_Max_Index = 6;
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
  pinMode(DS18B20_pin_number , INPUT);
  pinMode(ReedSwitch_pin_number , INPUT_PULLUP);
  pinMode(MQ5_pin_number , INPUT);
  pinMode(Buzzer_pin_number , OUTPUT);
  pinMode(UltraSonic_Trig_pin_number, OUTPUT);
  pinMode(UltraSonic_Echo_pin_number, INPUT);
  pinMode(has_detected_movement, INPUT);
  pinMode(Lamp_pin_number, OUTPUT);
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
  Get_water_level();
  Get_air_quality();
  Get_Reed_Switch();
  Get_Movement();
  //controlling the envirment
  Fan_Controll();
  Pump_controll();
  Control_Air_quality();
  Security_Controll();
  Lamp_Controll();
  //Display on LCD
  Toggle_Controll();
  Displayall();
  delay(3000);
}

void Get_temprature() {
  Serial.print(" Requesting temperatures...");
  Temprature_sensors.requestTemperatures(); // Send the command to get temperature readings
  Serial.println("DONE");
  Serial.print("Temperature is: ");
  float temp = Temprature_sensors.getTempCByIndex(0);
  Serial.print(temp);
  Temperature_C = temp;
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
  water_volume = M_PI * pow(water_tank_radios, 2) * water_level;
}


void Pump_controll() {
  if (is_soil_dry) {
    digitalWrite(pump_pin_number , HIGH);
    pump_state = true;
  } else {
    digitalWrite(pump_pin_number , LOW);
    pump_state = false;
  }
}

void Get_air_quality() {
  air_quality_value = analogRead(MQ5_pin_number);
  Serial.println("air quality : " + String(air_quality_value));
  if (air_quality_value <= 800) {
    is_air_breathable = true;
    Serial.println("Air is breathable");
  }
  else {
    is_air_breathable = false;
    Serial.println("Air is not breathable - gas leak dettected");
  }

}


void Control_Air_quality() {
  if (!is_air_breathable) {
    tone(Buzzer_pin_number, 1000);
    delay(2000);
    noTone(Buzzer_pin_number);
    delay(500);
  }
}

void Get_Reed_Switch() {
  Reed_Switch_status = digitalRead(ReedSwitch_pin_number);
  if (Reed_Switch_status == HIGH) {
    is_window_open = true;
  } else {
    is_window_open = false;
  }

}

void Security_Controll() {
  if (!is_window_open) {
    tone(Buzzer_pin_number, 2500);
    delay(1000);
    noTone(Buzzer_pin_number);
    delay(500);
  }
}


void Get_Movement() {
  int movement_status = digitalRead(PIR_pin_number);
  if (movement_status == HIGH) {
    has_detected_movement = true;
  } else {
    has_detected_movement = false;
  }
}

void Lamp_Controll() {
  if (has_detected_movement) {
    digitalWrite(Lamp_pin_number, HIGH);
  } else {
    digitalWrite(Lamp_pin_number, LOW);
  }
}

void Toggle_Controll() {
  int button_state = digitalRead(Display_Toggle_Controll_pin_number);
  if (was_button_active == false) {
    if (button_state == LOW) {
      was_button_active = true;
      //check max
      if(Display_Info_Index >= Display_Max_Index){
        Display_Info_Index = 0;
      } else{
        Display_Info_Index += 1;
      }
    }
  }
  if (was_button_active == true) {
    if (button_state == HIGH) {
      was_button_active = false;
    }
  }
}
void Displayall() {
  switch(Display_Info_Index){
    case 0:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Water Level:");
    lcd.setCursor(0,1);
    lcd.print(String(water_level) + " cm");
    break;
    case 1:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Water Volume:");
    lcd.setCursor(0,1);
    lcd.print(String(water_volume) + " cm^3");
    break;
    case 2:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temprature:");
    lcd.setCursor(0,1);
    lcd.print(String(Temperature_C) + " C");
    break;
    case 3:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Fan Speed:");
    lcd.setCursor(0,1);
    lcd.print(String(fanLCD) + " %");
    break;
    case 4:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Air quality:");
    lcd.setCursor(0,1);
    lcd.print(String(air_quality_value) + "");
    break;
    case 5:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Window status:");
    lcd.setCursor(0,1);
    lcd.print(String(is_window_open) + "");
    break;
    case 6:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("pump status:");
    lcd.setCursor(0,1);
    lcd.print(String(pump_state) + "");
    break;
    
  }

}
