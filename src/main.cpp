#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

/*char ssid[] = "IZZI-2124" ; 
char pass[] = "ECA9404C2124" ;
int status = WL_IDLE_STATUS;     // the Wifi radio's status*/

WebSocketsClient webSocket;

WiFiClient client;

StaticJsonDocument<200> jsonDocTx;

bool wsconnected = false;
bool mensaje_recibido = true; 

struct Quaternion {
    float QW; 
    float QX; 
    float QY; 
    float QZ; 
}; 

Quaternion desiredQuat; 
Quaternion currentQuat; 

void printWifiData() {

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
      wsconnected = false;
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
      wsconnected = true;
			// send message to server when Connected
			webSocket.sendTXT("Connected from ESP32"); 
      mensaje_recibido = false;
			break;
		case WStype_TEXT:
			// Serial.printf("[WSc] get text: %s\n", payload);
      mensaje_recibido = true; 
			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			//Serial.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}
}

void foundIP(void){
  // Esta función busca automáticamente el servidor disponible en el puerto
  // 5000 de varias IP's para localizar la IP del PC servidor
  IPAddress server(192, 168, 0, 2); 
  String iPdelPC = ""; 
  for(int i=0; i < 256; i++){
    bool connectedServer = false; 
    for(int j=2; j < 25; j++){
      iPdelPC = "192.168." + String(i) + "." + String(j) ; 
      server[2] = i; 
      server[3] = j; 
      if (client.connect(server, 5000)) {
        Serial.print("IP of PC: "); 
        Serial.println(server); 
        Serial.println("connected to server");
        connectedServer = true; 
        break;
      }
      Serial.println("Please wait, looking for the PC IP");
      delay(10) ; 
    }
    if(connectedServer){
      break ; 
    }
  }

  webSocket.begin(server, 8001, "/"); 
  
  webSocket.onEvent(webSocketEvent) ;
  webSocket.setReconnectInterval(50); 

  Serial.println("Websocket has begin"); 
}


void taskWifiInternet(void) {
  // Esta función inicia la conexión WIFI, y se conecta a un 
  // servidor Web Socket, esta función hace uso de un modem

  char ssid[] = "IZZI-2124" ; 
  char pass[] = "ECA9404C2124" ;
  int status = WL_IDLE_STATUS;     // the Wifi radio's status
  //int WL_CONNECTED = 0; 

  while (status != 6) {

    Serial.print("Attempting to connect to WPA SSID: ");
    status = WiFi.begin(ssid, pass);
    delay(3000);

  }

  Serial.print("You're connected to the network");

  printWifiData();

  Serial.println(WiFi.localIP());

  delay(2000) ; 

  foundIP(); 
}

void getAndSendInfo()
{
  //Declare a string where data is saved
  String output;

  //Resetar string output
  output = "";

  //Resetear documento JSON
  jsonDocTx.clear();

  //Almacenar info del quaternio en documento JSON
  jsonDocTx["QW"] = currentQuat.QW ; 
  jsonDocTx["QX"] = currentQuat.QX ; 
  jsonDocTx["QY"] = currentQuat.QY ; 
  jsonDocTx["QZ"] = currentQuat.QZ ; 

  /*jsonDocTx["Lx"] = buttons.Lx ; // Agregar lectura de botones al archivo JSON
  jsonDocTx["Ly"] = buttons.Ly ; 
  jsonDocTx["Lw"] = buttons.Lw ; 
  jsonDocTx["Rx"] = buttons.Rx ; 
  jsonDocTx["Ry"] = buttons.Ry ; 
  jsonDocTx["Rw"] = buttons.Rw ; */

  jsonDocTx["dQW"] = desiredQuat.QW; 
  jsonDocTx["dQX"] = desiredQuat.QX; 
  jsonDocTx["dQY"] = desiredQuat.QY; 
  jsonDocTx["dQZ"] = desiredQuat.QZ; 

  //Convertir documento JSON en un string
  serializeJson(jsonDocTx, output);

  //Enviar string con info del documento JSON
  webSocket.sendTXT(output);  
}

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

  desiredQuat.QW = 1; 
  desiredQuat.QX = 0; 
  desiredQuat.QY = 0; 
  desiredQuat.QZ = 0; 

  currentQuat.QW = 1; 
  currentQuat.QX = 0; 
  currentQuat.QY = 0; 
  currentQuat.QZ = 0; 

  webSocket.loop();
  getAndSendInfo() ; // Send all the info to the computer

  delay(100); 
}