#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SENSOR 5


TaskHandle_t xTask_sensor_handle = NULL;
TaskHandle_t xTasK_server_handle = NULL;


const char* ssid = "brisa-2195814";
const char* password = "ur9g4grt";

unsigned int cont;

const float FATOR_CALIBRACAO = 4.5;

float fluxo = 0;
float volume = 0;
float volume_total = 0;

unsigned long tempo_antes = 0;

void ISR_SENSOR();
void Wifi_setup();
void v_read_sensor(void * pvParameters);
void v_server(void * pvParameters);

WiFiServer sv(8080);


void setup() {
  xTaskCreate(v_read_sensor,"task sensor",configMINIMAL_STACK_SIZE+1024,NULL,1,&xTask_sensor_handle);
  xTaskCreate(v_server,"Server Task",configMINIMAL_STACK_SIZE+1024,NULL,1,&xTasK_server_handle);
  Serial.begin(9600);
  Wifi_setup();
  pinMode(SENSOR,INPUT);
  cont = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR),ISR_SENSOR,FALLING);
  sv.begin();
}

void loop() {
    if((millis() - tempo_antes) > 300){
          detachInterrupt(SENSOR);

          fluxo = ((300.0 / (millis() - tempo_antes)) * cont) / FATOR_CALIBRACAO;

          Serial.print(fluxo);
          Serial.println(" L/min");
          //Serial.println(WiFi.localIP());
          volume = fluxo / 60;
          volume_total += volume;
          cont = 0;
          tempo_antes = millis();
          attachInterrupt(SENSOR, ISR_SENSOR, FALLING);
    }
}

void v_server(void * pvParameters){
  while(1){
    WiFiClient client = sv.available();  
    if(client){
      while (client.connected()) {            
        if (client.available()){
          client.println("HTTP/1.1 200 OK");
          client.println();
          client.println(fluxo);
          client.println( "L/min");
        } 
      }
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
  }
  

}



void Wifi_setup(){
  WiFi.begin(ssid, password);
 
 while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 Serial.println(WiFi.localIP());
}

void v_read_sensor(void * pvParameters){
  while(1){
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


void ISR_SENSOR(){
  cont++;
}