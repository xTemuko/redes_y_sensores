#include <WiFi.h>


const char* ssid     = "Xtem";
const char* password = "trudi1818";


const uint16_t CHAT_PORT = 6000;

WiFiServer server(CHAT_PORT);
WiFiClient  client;

void setup() {
  Serial.begin(115200);
  delay(10);


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.printf("Conectando a '%s'", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n Wi-Fi conectado!");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());


  server.begin();
  Serial.printf("Servidor TCP escuchando en el puerto %u\n", CHAT_PORT);
}

void loop() {


  if ((!client || !client.connected()) && server.hasClient()) {
    if (client) client.stop();
    client = server.available();
    Serial.println(">> Cliente conectado al chat");
    client.println("¡Bienvenido!");
  }


  if (client && client.connected()) {


    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }


    while (Serial.available()) {
      char c = Serial.read();
      client.write(c);
    }
  }
}
