#include <WiFi.h>
#include <time.h>

const char* ssid     = "Xtem";
const char* password = "trudi1818";


const char* ntpServer       = "pool.ntp.org";
const long  gmtOffset_sec   = 3600;     
const int   daylightOffset_sec = 3600;  


void setup() {
  Serial.begin(115200);
  delay(10);


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.printf("Conectando a '%s' …", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n Wi-Fi conectada!");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Sincronizando hora con NTP…");
  if (!obtenerHora()) {
    Serial.println("Error al obtener hora NTP");
  }
}

void loop() {

  delay(10000);
  obtenerHora();
}

bool obtenerHora() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return false;
  }

  Serial.printf("Fecha y hora: %02d/%02d/%04d %02d:%02d:%02d\n",
    timeinfo.tm_mday,
    timeinfo.tm_mon + 1,
    timeinfo.tm_year + 1900,
    timeinfo.tm_hour,
    timeinfo.tm_min,
    timeinfo.tm_sec
  );
  return true;
}
