#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

BLEService accelService("D27A1000-761F-4F14-8A11-3BCB8D8EF0CE");  // UUID del servicio de acelerómetro
BLECharacteristic accelChar("D27A1001-761F-4F14-8A11-3BCB8D8EF0CE",  // UUID de la característica
                             BLERead | BLENotify, sizeof(float) * 3);

void setup() {
  Serial.begin(9600);
  if (!BLE.begin()) {
    Serial.println("Error iniciando BLE");
    while (1);
  }
  // Iniciar el sensor IMU (acelerómetro)
  if (!IMU.begin()) {
    Serial.println("Error: IMU no detectada");
  } else {
    Serial.println("IMU iniciada correctamente");
  }

  // Inicializar valor de la característica (ej. 0,0,0)
  float initialAccel[3] = {0.0, 0.0, 0.0};
  accelChar.writeValue((byte*)initialAccel, sizeof(initialAccel));

  // Configurar servicio BLE
  accelService.addCharacteristic(accelChar);
  BLE.addService(accelService);
  BLE.setAdvertisedService(accelService);
  BLE.setLocalName("Nano33-Accel");
  BLE.advertise();
  Serial.println("Periférico BLE iniciado: Acelerómetro con notificaciones.");
}

void loop() {
  // Esperar conexión de un central BLE
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Central conectado: ");
    Serial.println(central.address());
    while (central.connected()) {
      // Leer acelerómetro y notificar valores
      if (IMU.accelerationAvailable()) {
        float x, y, z;
        IMU.readAcceleration(x, y, z);
        float values[3] = {x, y, z};
        accelChar.writeValue((byte*)values, sizeof(values));  // actualiza y notifica
        Serial.print("Accel [g]: ");
        Serial.print(x, 3); Serial.print(", ");
        Serial.print(y, 3); Serial.print(", ");
        Serial.println(z, 3);
      }
      delay(500);  
    }
    Serial.println("Central desconectado");
  }
}
