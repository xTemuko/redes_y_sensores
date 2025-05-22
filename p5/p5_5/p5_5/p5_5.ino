#include <WiFi.h>
#include <time.h>


const char* ssid     = "Xtem";
const char* password = "trudi1818";



const char* ntpServer           = "pool.ntp.org";
const long  gmtOffset_sec       = 3600;    
const int   daylightOffset_sec  = 3600;    


const uint16_t CHAT_PORT = 6000;
WiFiServer server(CHAT_PORT);
WiFiClient  client;


bool sendingTime = false;
unsigned long lastSend = 0;
const unsigned long interval = 10000;  // 10 segundos

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
  Serial.println("\n¡Wi-Fi conectado!");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Sincronizando NTP…");
  struct tm tm;
  if (!getLocalTime(&tm)) {
    Serial.println("Error al obtener hora NTP");
  } else {
    Serial.println("Hora NTP obtenida correctamente");
  }


  server.begin();
  Serial.printf("Servidor TCP escuchando en puerto %u\n", CHAT_PORT);
}

void loop() {

  if ((!client || !client.connected()) && server.hasClient()) {
    if (client) client.stop();
    client = server.available();
    sendingTime = false;       // reset estado al reconectar
    Serial.println("Cliente CONECTADO");
    client.println("Bienvenido. Envía 'start' para recibir hora cada 10s, 'stop' para parar.");
  }


  if (client && client.connected()) {

    if (client.available()) {
      String cmd = client.readStringUntil('\n');
      cmd.trim();  
      Serial.printf("Comando recibido: '%s'\n", cmd.c_str());
      if (cmd.equalsIgnoreCase("start")) {
        sendingTime = true;
        lastSend = millis();  
        client.println("START recibido: enviando hora cada 10s.");
      }
      else if (cmd.equalsIgnoreCase("stop")) {
        sendingTime = false;
        client.println("STOP recibido: paro de enviar hora.");
      }
      else {
        client.printf("Comando desconocido: '%s'\n", cmd.c_str());
      }
    }

    if (sendingTime && millis() - lastSend >= interval) {
      lastSend = millis();
      enviarHora();
    }

    if (Serial.available()) {
      String out = Serial.readStringUntil('\n');
      if (out.length()) {
        client.println(out);
      }
    }
  }
}

// Función para obtener hora y mandarla
void enviarHora() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char buf[64];
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &timeinfo);
    client.printf("Hora: %s\n", buf);
    Serial.printf("Enviado hora: %s\n", buf);
  } else {
    client.println("ERROR: no se pudo leer hora NTP");
    Serial.println("ERROR: getLocalTime falló");
  }
}
