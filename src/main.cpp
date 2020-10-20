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

#define LED 2

AsyncWebServer server(80);

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

const char* PARAM_MESSAGE = "message";

void on_Index_Request(AsyncWebServerRequest *request){
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.print("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/index.html", "text/html");
}

void on_CSS_Request(AsyncWebServerRequest *request){
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.print("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/cover.css", "text/css");
}

void on_Bootstrap_Min_CSS_Request(AsyncWebServerRequest *request){
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.print("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/bootstrap.min.css", "text/css");
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {

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

    /*server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });
    */

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();
}

void loop() {
}