#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>


const char* ssid     = "Xtem";
const char* password = "trudi1818";



const char* tcpServerIP   = "192.168.1.100";  // IP de tu servidor TCP
const uint16_t tcpServerPort = 6000;          // Puerto de tu servidor TCP
WiFiClient tcpClient;


// —————— UUIDs BLE del Nano33 ——————
static BLEUUID serviceUUID("D27A1000-761F-4F14-8A11-3BCB8D8EF0CE");
static BLEUUID  charUUID("D27A1001-761F-4F14-8A11-3BCB8D8EF0CE");


BLEClient*  pBLEClient;
BLERemoteCharacteristic* pAccelChar;
bool        doConnect     = false;
bool        connectedBLE  = false;
bool        doScan        = false;
static BLEAddress *pServerAddress;

// Callback notificación BLE
static void accelNotifyCallback(
  BLERemoteCharacteristic* ,
  uint8_t* data,
  size_t length,
  bool /*isNotify*/
) {
  if (length != sizeof(float)*3) return;
  float accel[3];
  memcpy(accel, data, length);

  // Reconectar TCP si es necesario
  if (!tcpClient.connected()) {
    if (tcpClient.connect(tcpServerIP, tcpServerPort)) {
      Serial.println("TCP: Conectado al servidor");
    } else {
      Serial.println("TCP: Error al conectar");
      return;
    }
  }

  // Enviar x,y,z formateado CSV
  tcpClient.printf("%.3f,%.3f,%.3f\n", accel[0], accel[1], accel[2]);
  Serial.printf("Enviado TCP: %.3f, %.3f, %.3f\n", accel[0], accel[1], accel[2]);
}

// Intento de conexion al  BLE
bool connectToServer() {
  Serial.print("BLE: Conectando a ");
  Serial.println(pServerAddress->toString().c_str());
  pBLEClient = BLEDevice::createClient();
  if (!pBLEClient->connect(*pServerAddress)) {
    Serial.println("BLE: fallo al conectar");
    return false;
  }
  BLERemoteService* accelService = pBLEClient->getService(serviceUUID);
  if (!accelService) {
    Serial.println("BLE: servicio no encontrado");
    pBLEClient->disconnect();
    return false;
  }
  pAccelChar = accelService->getCharacteristic(charUUID);
  if (!pAccelChar) {
    Serial.println("BLE: característica no encontrada");
    pBLEClient->disconnect();
    return false;
  }
  // Suscribirse a notificaciones
  if(pAccelChar->canNotify()) {
    pAccelChar->registerForNotify(accelNotifyCallback);
  }
  Serial.println("BLE: suscrito a notificaciones de acelerómetro");
  return true;
}

// Callback de scan
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.isAdvertisingService(serviceUUID)) {
      Serial.println("BLE: dispositivo encontrado!");
      BLEDevice::getScan()->stop();
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      doConnect = true;
      doScan = false;
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(1000);


  Serial.printf("WiFi: Conectando a '%s' …", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP local: "); Serial.println(WiFi.localIP());


  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  doScan = true;
}

void loop() {

  if (doConnect) {
    if (connectToServer()) {
      connectedBLE = true;
    } else {
      connectedBLE = false;
    }
    doConnect = false;
  }


  if (!connectedBLE && !doScan) {
    BLEDevice::getScan()->start(0);  
    doScan = true;
  }


  if (connectedBLE && pBLEClient && !pBLEClient->isConnected()) {
    Serial.println("BLE: desconectado, reescaneando...");
    connectedBLE = false;
    delete pBLEClient;
    doScan = true;
  }


  delay(100);
}
