#include <ArduinoBLE.h>

void setup() {
  Serial.begin(9600);
  // Iniciar BLE
  if (!BLE.begin()) {
    Serial.println("Error iniciando BLE");
    while (1);
  }

  BLEService beaconService("4D414E55-454C-0000-0000-000000000000");  
  BLE.setDeviceName("Nano33BLE");
  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(beaconService);
  BLE.addService(beaconService);
  BLE.advertise();
  Serial.println("Beacon BLE iniciado, anunciando UUID personalizado.");
}

void loop() {
}
