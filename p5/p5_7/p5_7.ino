#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <sys/time.h>

const char* ssid              = "MOVISTAR_DB78";
const char* password          = "iqbenVcwzhEsEofhjFWC";
const char* ntpServer         = "pool.ntp.org";
const long  gmtOffset_sec     = 3600;   // GMT+1
const int   daylightOffset_sec= 3600;   // +1 hora verano

WebServer server(80);


void handleRoot() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    server.send(500, "text/plain", "Error al leer hora");
    return;
  }


  char timeStr[32];
  sprintf(timeStr,
    "%02d/%02d/%04d %02d:%02d:%02d",
    timeinfo.tm_mday,
    timeinfo.tm_mon + 1,
    timeinfo.tm_year + 1900,
    timeinfo.tm_hour,
    timeinfo.tm_min,
    timeinfo.tm_sec
  );

  String html =  
    "<!DOCTYPE html><html><head>"
      "<meta charset='UTF-8'>"
      "<meta http-equiv='refresh' content='1'>"  // recarga cada segundo
      "<title>ESP32 Reloj</title>"
    "</head><body>"
      "<h1>Hora actual</h1>"
      "<p style='font-size:2em;'>" + String(timeStr) + "</p>"
      "<form action='/reset' method='POST'>"
        "<button style='font-size:1.2em;padding:0.5em;'>"
          "Resetear a 00:00"
        "</button>"
      "</form>"
    "</body></html>";

  server.send(200, "text/html", html);
}

void handleReset() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    server.send(500, "text/plain", "Error al leer hora");
    return;
  }

  time_t now      = time(nullptr);
  time_t midnight = now
    - (timeinfo.tm_hour * 3600
     + timeinfo.tm_min  *   60
     + timeinfo.tm_sec);


  timeval tv = { .tv_sec = midnight, .tv_usec = 0 };
  settimeofday(&tv, nullptr);


  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

void setup() {
  Serial.begin(115200);
  delay(500);


  Serial.printf("Conectando a Wi-Fi '%s' …", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.printf("\nWi-Fi OK. IP: %s\n", WiFi.localIP().toString().c_str());


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Sincronizando hora NTP…");
  struct tm tmp;
  if (!getLocalTime(&tmp)) {
    Serial.println("¡Fallo NTP!");
  } else {
    Serial.println("Hora NTP OK");
  }

  server.on("/",    HTTP_GET,  handleRoot);
  server.on("/reset",HTTP_POST, handleReset);
  server.begin();
  Serial.println("Servidor web iniciado en puerto 80");
}

void loop() {
  server.handleClient();
}

