#include <WiFi.h>
#include <WebServer.h>      // เพิ่มตัวนี้มาส่งหน้าเว็บ
#include <WebSocketsServer.h>
#include "index.h"

#define LED 27
const char* ssid     = "@JumboPlusIoT";
const char* password = "hr92o378";

// ตั้งค่า Port: HTTP ใช้ 80, WebSocket ใช้ 81 (แยกกันเพื่อไม่ให้ชนกัน)
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      Serial.printf("[%u] Connected!\n", num);
      webSocket.sendTXT(num, "Connected to Cube"); // ส่งข้อความทักทาย
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      break;
  }
  switch(type) {
    case WStype_TEXT:
      String message = String((char*)(payload)); // แปลงข้อความที่รับมาเป็น String
      Serial.printf("Received: %s\n", message.c_str());

      if (message == "ON") {
        digitalWrite(LED, HIGH); // เปิดไฟ
      } else if (message == "OFF") {
        digitalWrite(LED, LOW);  // ปิดไฟ
      }
      break;
    
    // ... เคสอื่นๆ (CONNECTED/DISCONNECTED) คงไว้ตามเดิม ...
  }
}

// ฟังก์ชันสำหรับส่งหน้าเว็บ index.h
void handleRoot() {
  server.send(200, "text/html", index_html); 
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500); Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());

  // --- ส่วนของ HTTP Server ---
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  // --- ส่วนของ WebSocket ---
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  Serial.println("WebSocket server started on port 81");

  pinMode(LED, OUTPUT);
}

void loop() {
  server.handleClient(); // จัดการ HTTP request
  webSocket.loop();      // จัดการ WebSocket data

  // ทดสอบส่งค่า Random ทุกๆ 100ms
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 100) {
    lastMsg = millis();
    String json = "{\"x\":" + String(random(-10, 10)) + ",\"y\":" + String(random(-10, 10)) + "}";
    webSocket.broadcastTXT(json); // ส่งหาทุกเครื่องที่ต่ออยู่
  }
}
