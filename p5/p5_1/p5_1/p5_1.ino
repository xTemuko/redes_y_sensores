#include <WiFi.h>
#include <ESPping.h>

const char* ssid     = "Xtem";
const char* password = "trudi1818";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado!");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());
  hacerPing();
}

void loop() {
  delay(10000);
  hacerPing();
}

void hacerPing() {
  Serial.print("Ping a google.com … ");
  if (Ping.ping("google.com")) {
    Serial.print("OK, media: ");
    Serial.print(Ping.averageTime());
    Serial.println(" ms");
  } else {
    Serial.println("Fallo");
  }
}

