//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <WebSocketsServer.h>

#include <ws2812.h>

#define PIN_RGB 18
#define NUM_PIXELS  2
rgbVal *pixels;

#define LED 2

//WS2812B
int r,g,b;
char numero[3]="";
int posiaux, aux;

bool estadoLed;

AsyncWebServer server(80);
WebSocketsServer ws(81);

//Coneccion Wifi
const char *ssid = "Telecentro-2fd0     ";
const char *password = "EDZKVDNWCZYW";

// it wil set the static IP address to 192, 168, 1, 184
IPAddress local_IP(192, 168, 0, 184);
//it wil set the gateway static IP address to 192, 168, 1,1
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(0, 0, 0, 0); 
IPAddress secondaryDNS(0, 0, 0, 0);

char msg_buf[10];

void displayOff(void);

// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = ws.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;

    // Handle text messages from client
    case WStype_TEXT:

      // Print out raw message
      Serial.printf("[%u] Received text: %s\n", client_num, payload);

      // Toggle LED
      if ( strcmp((char *)payload, "toggleLED") == 0 ) {
        estadoLed = estadoLed ? 0 : 1;
        Serial.printf("Toggling LED to %u\n", estadoLed);
        digitalWrite(LED, estadoLed);

      // Report the state of the LED
      } else if ( strcmp((char *)payload, "getLEDState") == 0 ) {
        sprintf(msg_buf, "%d", estadoLed);
        Serial.printf("Sending to [%u]: %s\n", client_num, msg_buf);
        //ws.sendTXT(client_num, msg_buf);
        ws.broadcastTXT(msg_buf);

      // Message not recognized
      } else if(strstr((char*)payload,"rgb")) {

          numero[0]=NULL;
          numero[1]=NULL;
          numero[2]=NULL;
          for(int i=4;payload[i]!=',';i++) {
              numero[i-4]=payload[i];
              posiaux = i;
          }
          
          r = atoi(numero);

          numero[0]=NULL;
          numero[1]=NULL;
          numero[2]=NULL;
          aux = posiaux+3;
          for(int i=aux;payload[i]!=',';i++) {
              numero[i-aux]=payload[i];
              posiaux = i;
          }
          
          g = atoi(numero);

          numero[0]=NULL;
          numero[1]=NULL;
          numero[2]=NULL;
          aux = posiaux+3;
          for(int i=aux;payload[i]!=')';i++) {
              numero[i-aux]=payload[i];
              posiaux = i;
          }
          
          b = atoi(numero);

          displayOff();

          for (int i = 0; i < NUM_PIXELS; i++) {
              pixels[i] = makeRGBVal(r, g, b);
          }
          ws2812_setColors(NUM_PIXELS, pixels);
      } else {
        Serial.println("[%u] Message not recognized");
      }
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void on_Index_Request(AsyncWebServerRequest *request){
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/index.html", "text/html");
}

void on_CSS_Request(AsyncWebServerRequest *request){
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/cover.css", "text/css");
}

void on_Bootstrap_Min_CSS_Request(AsyncWebServerRequest *request){
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/bootstrap.min.css", "text/css");
}

void on_Icon_Request(AsyncWebServerRequest *request){
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
}

void on_LED_ON_Request(AsyncWebServerRequest *request){
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/LED_ON.png", "image/png");
}

void on_LED_OFF_Request(AsyncWebServerRequest *request){
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/LED_OFF.png", "image/png");
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void on_bootstrap_min_js_request(AsyncWebServerRequest *request) {
	IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
	request->send(SPIFFS, "/bootstrap.min.js", "application/javascript");
}

void on_jquery_3_2_1_slim_min_js_request(AsyncWebServerRequest *request) {
	IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
	request->send(SPIFFS, "/jquery-3.2.1.slim.min.js", "application/javascript");
}

void on_popper_min_js_request(AsyncWebServerRequest *request) {
	IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
	request->send(SPIFFS, "/popper.min.js", "application/javascript");
}

void on_funciones_request(AsyncWebServerRequest *request) {
	IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
	request->send(SPIFFS, "/funciones.js", "application/javascript");
}

void setup() {

    pinMode(LED, OUTPUT);
    digitalWrite(LED, 0);
    estadoLed = digitalRead(LED);
    

    Serial.begin(9600);

    if (!SPIFFS.begin()){
        Serial.println("Error mounting SPIFFS");
    }

    WiFi.mode(WIFI_STA);
    // This part of code will try create static IP address
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)){
        Serial.println("STA Failed to configure");
    }
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, on_Index_Request);
    server.on("/cover.css", HTTP_GET, on_CSS_Request);
    server.on("/bootstrap.min.css", HTTP_GET, on_Bootstrap_Min_CSS_Request);
    server.on("/favicon.ico", HTTP_GET, on_Icon_Request);
    server.on("/LED_ON.png", HTTP_GET, on_LED_ON_Request);
    server.on("/LED_OFF.png", HTTP_GET, on_LED_OFF_Request);
	server.on("/bootstrap.min.js", HTTP_GET, on_bootstrap_min_js_request);
	server.on("/jquery-3.2.1.slim.min.js", HTTP_GET, on_jquery_3_2_1_slim_min_js_request);
	server.on("/popper.min.js", HTTP_GET, on_popper_min_js_request);
	server.on("/funciones.js", HTTP_GET, on_funciones_request);

    server.onNotFound(notFound);

    server.begin();
    ws.begin();
    ws.onEvent(onWebSocketEvent);
    ws2812_init(PIN_RGB, LED_WS2812);
    pixels = (rgbVal*)malloc(sizeof(rgbVal) * NUM_PIXELS);
    displayOff();
}

void loop() {
    ws.loop();
}

void displayOff() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels[i] = makeRGBVal(0, 0, 0);
  }
  ws2812_setColors(NUM_PIXELS, pixels);
}