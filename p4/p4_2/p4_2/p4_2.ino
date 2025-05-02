#include <ArduinoBLE.h>



BLECharacteristic colorChar("19B10001-E8F2-537E-4F6C-D104768A1214", BLEWrite, 3);

void setup() {
  Serial.begin(9600);
  while (!Serial);  
  if (!BLE.begin()) {
    Serial.println("Error iniciando BLE");
    while (1);
  }


  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);



  BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");
  ledService.addCharacteristic(colorChar);
  BLE.addService(ledService);
  BLE.setAdvertisedService(ledService);
  BLE.setLocalName("LEDColor");
  BLE.advertise();
  Serial.println("Periférico BLE listo.");
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Central conectado: ");
    Serial.println(central.address());
    while (central.connected()) {
      if (colorChar.written()) {
        byte rgb[3];
        colorChar.readValue(rgb, 3);
        Serial.print("RGB recibido: ");
        Serial.print(rgb[0]); Serial.print(", ");
        Serial.print(rgb[1]); Serial.print(", ");
        Serial.println(rgb[2]);

        analogWrite(LEDR, 255 - rgb[0]);
        analogWrite(LEDG, 255 - rgb[1]);
        analogWrite(LEDB, 255 - rgb[2]);

      }
      BLE.poll();
    }
    Serial.println("Central desconectado");
  }
}
