#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "index.h"

// กำหนดขา LED
#define LED_PIN 27

// ข้อมูล WiFi
const char* ssid     = "@JumboPlusIoT";
const char* password = "hr92o378";

// สร้าง Instance สำหรับ Server
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// ฟังก์ชันจัดการเหตุการณ์ WebSocket
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %s\n", num, ip.toString().c_str());
      webSocket.sendTXT(num, "Connected to Cube");
      break;
    }

    case WStype_TEXT: {
      String message = String((char*)(payload));
      Serial.printf("[%u] Received: %s\n", num, message.c_str());

      // ควบคุม LED และส่งสถานะกลับไปยืนยัน
      if (message == "ON") {
        digitalWrite(LED_PIN, HIGH);
        webSocket.broadcastTXT("{\"status\":\"LED is ON\"}");
      } else if (message == "OFF") {
        digitalWrite(LED_PIN, LOW);
        webSocket.broadcastTXT("{\"status\":\"LED is OFF\"}");
      }
      break;
    }
  }
}

void handleRoot() {
  server.send(200, "text/html", index_html); 
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // ตั้งค่า WiFi ให้เสถียร
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true); // ต่อใหม่อัตโนมัติเมื่อหลุด
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected! IP: %s\n", WiFi.localIP().toString().c_str());

  server.on("/", handleRoot);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  
  // ตรวจสอบ Client ที่ค้าง (Heartbeat) ทุกๆ 15 วินาที
  webSocket.enableHeartbeat(15000, 3000, 2); 
}

void loop() {
  server.handleClient();
  webSocket.loop();

  // ส่งข้อมูลพิกัดสุ่มทุก 100ms
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 100) {
    lastMsg = millis();
    
    // ส่งเฉพาะตอนที่มีคนเชื่อมต่ออยู่ เพื่อประหยัดทรัพยากร
    if (webSocket.connectedClients() > 0) {
        String json;
        json.reserve(64); // จองพื้นที่ RAM ป้องกัน Memory Fragmentation
        json = "{\"x\":" + String(random(-10, 10)) + ",\"y\":" + String(random(-10, 10)) + "}";
        webSocket.broadcastTXT(json);
    }
  }
}
