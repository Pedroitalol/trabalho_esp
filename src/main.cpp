#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>


#define SENSOR 5

const char* ssid = "brisa-2195814";
const char* password = "ur9g4grt";

unsigned int cont;

const float FATOR_CALIBRACAO = 4.5;

float fluxo = 0;
float volume = 0;
float volume_total = 0;

unsigned long tempo_antes = 0;

void ISR_SENSOR();

WebServer sv(80);


void setup() {
  Serial.begin(9600);
  pinMode(SENSOR,INPUT);
  cont = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR),ISR_SENSOR,FALLING);
}

void loop() {
  if((millis() - tempo_antes) > 300){

    detachInterrupt(SENSOR);

    fluxo = ((300.0 / (millis() - tempo_antes)) * cont) / FATOR_CALIBRACAO;

    Serial.print(fluxo);
    Serial.println(" L/min");

    volume = fluxo / 60;

    volume_total += volume;

    Serial.print(volume_total);
    Serial.println(" L");
    cont = 0;
    tempo_antes = millis();

    attachInterrupt(SENSOR, ISR_SENSOR, FALLING);
    
  }
  
}


void ISR_SENSOR(){
  cont++;
}