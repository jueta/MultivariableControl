#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <stdio.h>
#include <string.h>

#define DHTPIN  15   // Digital pin connected to the DHT sensor 
#define RELAY 13
#define COOLER 12
#define DHTTYPE    DHT11     // DHT 11
#define PWM_COOLER    0
#define PWM_LUZ    1
#define PWM_Resolution   10
#define PWM_RELAY_FREQ  60
#define PWM_COOLER_FREQ  10000

uint32_t delayMS;
int speedCooler = 0; 
int pwmLampada = 1024; 

DHT_Unified dht(DHTPIN, DHTTYPE);


// Controlador PID da Temperatura
double temp_Setpoint = 30;
unsigned long temp_currentTime, temp_previousTime;
double temp_elapsedTime;
double temp_lastError;
double temp_Input, temp_Output;
double temp_error, temp_cumError, temp_rateError;
double temp_Kp = 5.22;
double temp_Ki = 0.282;
double temp_Kd = -7.62;

// Controlador PID da Umidade
double umi_Setpoint = 70;
unsigned long umi_currentTime, umi_previousTime;
double umi_elapsedTime;
double umi_lastError;
double umi_Input, umi_Output;
double umi_error, umi_cumError, umi_rateError;
double umi_Kp = 6.08;
double umi_Ki = 0.419;
double umi_Kd = -6.4;

double computeTemp(){

  temp_currentTime = millis();
  temp_elapsedTime = (double)(temp_currentTime - temp_previousTime);

  temp_error = temp_Setpoint - temp_Input;
  temp_cumError += temp_error*temp_elapsedTime;
  temp_rateError = (temp_error - temp_lastError)/temp_elapsedTime;

  double temp_Output = temp_Kp*temp_error + temp_Ki*temp_cumError + temp_Kd*temp_rateError;

  temp_lastError = temp_error;
  temp_previousTime = temp_currentTime;

  if(temp_Output < 0){
    temp_Output = 0;
  }
  else if(temp_Output > 100){
    temp_Output = 100;
  }

  return temp_Output;
}

double computeUmi(){
  
  umi_currentTime = millis();
  umi_elapsedTime = (double)(umi_currentTime - umi_previousTime);

  umi_error = umi_Setpoint - umi_Input;
  umi_cumError += umi_error*umi_elapsedTime;
  umi_rateError = (umi_error - umi_lastError)/umi_elapsedTime;

  double umi_Output = umi_Kp*umi_error + umi_Ki*umi_cumError + umi_Kd*umi_rateError;

  umi_lastError = umi_error;
  umi_previousTime = umi_currentTime;

  if(umi_Output < -100){
    umi_Output = -100;
  }
  else if(umi_Output > 0){
    umi_Output = 0;
  }

  return umi_Output;
}

void setup() {

  Serial.begin(9600);

  dht.begin();

  //configurando PWM cooler
  ledcAttachPin(COOLER, PWM_COOLER);
  ledcSetup(PWM_COOLER, PWM_COOLER_FREQ, PWM_Resolution);
  ledcWrite(PWM_COOLER, speedCooler);

  //configurando PWM lampada
  ledcAttachPin(RELAY, PWM_LUZ);
  ledcSetup(PWM_LUZ, PWM_RELAY_FREQ, PWM_Resolution);
  ledcWrite(PWM_LUZ, pwmLampada);

  // Configurando Sensor
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  
}

void loop() {

  delay(delayMS);
  sensors_event_t event;

  // Input de SetPoint
  if (Serial.available()) {
    char spd = Serial.read();
    if(spd == 't'){
      int value = Serial.read();
      temp_Setpoint = value;
    }
    if(spd == 'u') {
      int value = Serial.read();
      umi_Setpoint = value;
    }
  }

//  temp_Output = computeTemp();
//  umi_Output = computeUmi();
//
//  Serial.print(temp_Output);
//  Serial.print(", ");
//  Serial.print(-umi_Output);
//  Serial.print(", ");
//
//  dht.temperature().getEvent(&event);
//  if (isnan(event.temperature)) {
//  }
//  else {
//    temp_Input = event.temperature;
//    Serial.print(temp_Input);
//    Serial.print(", ");  
//  }
//
//  dht.humidity().getEvent(&event);
//  if (isnan(event.relative_humidity)) {
//  }
//  else {
//    umi_Input = event.relative_humidity;
//    Serial.print(umi_Input);
//  }
//  Serial.println();

 Serial.print("Set Point Temperatura: ");
 Serial.println(temp_Setpoint); 
 Serial.print("Set Point Umidade: ");
 Serial.println(umi_Setpoint); 


// Input Temperatura
 dht.temperature().getEvent(&event);
 if (isnan(event.temperature)) {
 }
 else {
   temp_Input = event.temperature;
   Serial.print("Input Temperatura: ");
   Serial.print(temp_Input);
   Serial.print(", ");  
   Serial.println();
 }


// Input Umidade
 dht.humidity().getEvent(&event);
 if (isnan(event.relative_humidity)) {
 }
 else {
   umi_Input = event.relative_humidity;
   Serial.print("Input Umidade: ");
   Serial.print(umi_Input);
   Serial.println();
 }
 Serial.println();

// // Calculos
 temp_Output = computeTemp();
 umi_Output = computeUmi();


// PWM Lampada
 Serial.print("Output Lampada: ");
 Serial.print(temp_Output); 
  ledcWrite(PWM_LUZ, map(temp_Output, 0, 100, 1024, 0));
 Serial.print(", "); 
 Serial.println();

//PWM Ventilador
 Serial.print("Output Ventilador(%): ");
 Serial.print(umi_Output);  
  ledcWrite(PWM_COOLER, map(umi_Output, 0, -100, 0, 1024));
 Serial.print(", ");
 Serial.println();
 Serial.println("---------------------");


}
