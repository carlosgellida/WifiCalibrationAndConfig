#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <BasicLinearAlgebra.h>


using namespace BLA;

char ssid[] = "IZZI-2124" ; 
char pass[] = "ECA9404C2124" ;
//int status = WL_IDLE_STATUS;     // the Wifi radio's status*/


Matrix<4, 1> CurrQuaternion, DesiQuaternion; 

#include <WebPersonalLibrary.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicializar I2C
  //Wire.begin(); 

  delay(500) ; 

  // Inicializar WiFi y WebSocket
  //taskWifi() ; // Cambiar para utilizar sin modem
  taskWifiInternet(); 
}


void loop() {

  CurrQuaternion = {1, 0, 0, 0}; 

  DesiQuaternion = {1, 0, 0, 0}; 

  webSocket.loop();

  getAndSendInfo() ; // Send all the info to the computer

  delay(100); 
}