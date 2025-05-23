#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEClient.h>

const char* ssid     = "Xtem";
const char* password = "trudi1818";

const uint16_t TCP_PORT = 9000;
WiFiServer tcpServer(TCP_PORT);
WiFiClient tcpClient;            // cliente entrante

static BLEUUID serviceUUID("D27A1000-761F-4F14-8A11-3BCB8D8EF0CE");
static BLEUUID charUUID   ("D27A1001-761F-4F14-8A11-3BCB8D8EF0CE");

BLEScan*                pBLEScan;
BLEAdvertisedDevice*    myDevice    = nullptr;
bool                    doConnect   = false;
BLEClient*              pClient     = nullptr;
BLERemoteCharacteristic* pRemoteChar = nullptr;

static void notifyCallback(
  BLERemoteCharacteristic*, uint8_t* data, size_t length, bool
) {
  if (length != sizeof(float)*3) return;
  float a[3];
  memcpy(a, data, length);

  if (!tcpClient || !tcpClient.connected()) {
    tcpClient = tcpServer.available();
    if (tcpClient) {
      Serial.println("TCP: Cliente conectado al servidor ESP32");
    }
  }
  if (tcpClient && tcpClient.connected()) {
    tcpClient.printf("%.3f,%.3f,%.3f\n", a[0], a[1], a[2]);
    Serial.printf("Enviado TCP→Cliente: %.3f, %.3f, %.3f\n",
                  a[0], a[1], a[2]);
  }
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    Serial.println(advertisedDevice.toString().c_str());
    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.isAdvertisingService(serviceUUID))
    {
      Serial.println("BLE: encontrado Nano33-Accel!");
      pBLEScan->stop();
      myDevice  = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(500);

  // 1) Conectar Wi-Fi
  Serial.printf("WiFi: conectando a '%s' …", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.printf("\nWiFi OK. IP ESP32: %s\n", WiFi.localIP().toString().c_str());

  tcpServer.begin();
  Serial.printf("Servidor TCP escuchando en puerto %u\n", TCP_PORT);


  BLEDevice::init("ESP32-S3");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(0, nullptr, false);
}

void loop() {

  if (doConnect && myDevice) {
    Serial.print("BLE: conectando a ");
    Serial.println(myDevice->getAddress().toString().c_str());

    pClient = BLEDevice::createClient();
    if (pClient->connect(myDevice)) {
      Serial.println("BLE: conectado al periférico");
      auto svc = pClient->getService(serviceUUID);
      if (svc) {
        pRemoteChar = svc->getCharacteristic(charUUID);
        if (pRemoteChar && pRemoteChar->canNotify()) {
          pRemoteChar->registerForNotify(notifyCallback);
          Serial.println("BLE: suscrito a notificaciones");
        }
      }
    } else {
      Serial.println("BLE: fallo al conectar");
      pClient->disconnect();
    }
    doConnect = false;
  }

  if (pClient && !pClient->isConnected()) {
    Serial.println("BLE: desconectado, reescaneando...");
    delete pClient;
    myDevice = nullptr;
    pBLEScan->start(0, nullptr, false);
  }

  delay(50);
}

