#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include <esp_task_wdt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SENSOR 5


TaskHandle_t xTasK_server_handle = NULL;


const char* ssid = "brisa-2195814";
const char* password = "ur9g4grt";
String html = "";
unsigned int cont;

const float FATOR_CALIBRACAO = 4.5;

float fluxo = 0;
float volume = 0;
float volume_total = 0;

unsigned long tempo_antes = 0;

void html_create();
void ISR_SENSOR();
void Wifi_setup();
void v_read_sensor(void * pvParameters);
void v_server(void * pvParameters);
String createJsonString(float fluxo_atual);
WiFiServer sv(8080);


void setup() {
  xTaskCreate(v_server,"Server Task",configMINIMAL_STACK_SIZE+1024,NULL,1,&xTasK_server_handle);
  Serial.begin(9600);
  html_create();
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
          //queryString = createJsonString(fluxo);
          Serial.print(fluxo);
          Serial.println(" L/min");
          //Serial.println(WiFi.localIP());
          volume = fluxo / 60;
          volume_total += volume;
          html_create();
          cont = 0;
          tempo_antes = millis();
          
          attachInterrupt(SENSOR, ISR_SENSOR, FALLING);
    }
    vTaskDelay(10);
}

unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
String header;
String output26State = "off";
String output27State = "off";

void v_server(void * pvParameters){
  while(1){
    WiFiClient client = sv.available();   // Listen for incoming clients

    if (client) {                             // If a new client connects,
      currentTime = millis();
      previousTime = currentTime;
      Serial.println("New Client.");          // print a message out in the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
        currentTime = millis();
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          header += c;
          if (c == '\n') {                    // if the byte is a newline character
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
            

              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" charset=\"utf-8\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              // CSS to style the on/off buttons 
              // Feel free to change the background-color and font-size attributes to fit your preferences
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println(".button2 {background-color: #555555;}</style></head>");
              
              // Web Page Heading
              client.println("<body><h1>Vazão e volume de água</h1> ");
              
              // Display current state, and ON/OFF buttons for GPIO 26  
              client.println("<h2> Fluxo: "+String(fluxo)+" </h2><br><h2> Total: "+String(volume_total)+" </h2>");
              // If the output26State is off, it displays the ON button       
              
              client.println("<script> setInterval(recarregar, 1000); function recarregar(){ location.reload(); console.log('meu pai') } </script></body></html>");
              client.println();
              break;
            } else { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
        }
      }
      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
      vTaskDelay(10);
  }
}



void html_create(){
  html ="<!DOCTYPE html><html><head><title>Document</title></head><body><h2> Fluxo: "+String(fluxo)+" </h2><br><h2> Volume Total: "+String(volume)+" </h2></body></html>"; 
}

void Wifi_setup(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
 while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 Serial.println(WiFi.localIP());
}




void ISR_SENSOR(){
  cont++;
}

