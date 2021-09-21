/* Code created by Carlos Gellida, contact me at my email: gellidao@gmail.com */

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <BasicLinearAlgebra.h>


using namespace BLA;

//If your want to connect to a Modem add your credentials bellow:
char ssid[] = "IZZI-2124" ; 
char pass[] = "ECA9404C2124" ;

//If instead you want to use the ESP32 as an acces point, It will be work with 
//following credentials, feel free to modify them
const char* ssid2     = "ESP32-Access-Point";
const char* password2 = "123456789";


Matrix<4, 1> CurrQuaternion, DesiQuaternion; 

#include <WebPersonalLibrary.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicializar I2C
  //Wire.begin(); 

  delay(500) ; 

  // Inicializar WiFi y WebSocket

  // Descomentar lineas de abajo en relación a si se tiene o no un modem disponible
  //taskWifiNoModem() ; 
  taskWifiWithModem(); 

}


void loop() {

  CurrQuaternion = {1, 0, 0, 0}; 

  DesiQuaternion = {1, 0, 0, 0}; 

  webSocket.loop();

  getAndSendInfo() ; // Send all the info to the computer

  delay(100); 
}