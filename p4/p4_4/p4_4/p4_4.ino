#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

BLEService imuService("5FC16700-3FB0-42AE-A33B-7630E3EB22FD");  // UUID servicio IMU
BLECharacteristic accelChar("5FC16701-3FB0-42AE-A33B-7630E3EB22FD",  // caracteristica datos
                             BLERead | BLENotify, sizeof(float) * 3);
BLEByteCharacteristic controlChar("5FC16702-3FB0-42AE-A33B-7630E3EB22FD",  // caracteristica control
                                   BLEWrite);  // 1 byte (0=off, 1=on)

bool accelEnabled = true;  // flag de estado de captura (activo por defecto)

void setup() {
  Serial.begin(9600);
  if (!BLE.begin()) {
    Serial.println("Error iniciando BLE");
    while (1);
  }
  if (!IMU.begin()) {
    Serial.println("IMU no detectada");
  } else {
    Serial.println("IMU iniciada");
  }

  // Inicializar características BLE
  float initData[3] = {0.0, 0.0, 0.0};
  accelChar.writeValue((byte*)initData, sizeof(initData));
  controlChar.writeValue(1);  // valor inicial (1 = activado)

  imuService.addCharacteristic(accelChar);
  imuService.addCharacteristic(controlChar);
  BLE.addService(imuService);
  BLE.setAdvertisedService(imuService);
  BLE.setLocalName("Nano33-IMU-Ctrl");
  BLE.advertise();
  Serial.println("Periférico BLE (IMU c/ control) iniciado.");
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Central conectado: ");
    Serial.println(central.address());
    while (central.connected()) {
      // Revisar escritura en característica de control
      if (controlChar.written()) {
        if (controlChar.value() == 1) {         // valor escrito == 1
          accelEnabled = true;
          Serial.println("Captura de acelerometro ACTIVADA");
        } else {                                // valor 0
          accelEnabled = false;
          Serial.println("Captura de acelerometro DESACTIVADA");
        }
      }
      // Enviar datos de acelerómetro solo si está activado
      if (accelEnabled && IMU.accelerationAvailable()) {
        float x, y, z;
        IMU.readAcceleration(x, y, z);
        float accVals[3] = {x, y, z};
        accelChar.writeValue((byte*)accVals, sizeof(accVals));
        Serial.print("Accel: ");
        Serial.print(x, 3); Serial.print(", ");
        Serial.print(y, 3); Serial.print(", ");
        Serial.println(z, 3);
      }
      delay(100);  // limitar frecuencia de muestreo
    }
    Serial.println("Central desconectado");
  }
}
