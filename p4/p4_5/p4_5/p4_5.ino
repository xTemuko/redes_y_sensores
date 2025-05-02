#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth no está habilitado! Actívalo en la configuración del ESP32."
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_CHAT");  // Nombre Bluetooth del ESP32
  Serial.println("Bluetooth Classic iniciado, esperando conexión...");
}

void loop() {
  // Si llega datos desde la app Bluetooth
  if (SerialBT.available()) {
    String mensaje = SerialBT.readStringUntil('\n');  // leer hasta fin de línea
    Serial.print("Recibido via BT: ");
    Serial.println(mensaje);
    // Enviar respuesta de vuelta por Bluetooth
    String respuesta = "ESP32: " + mensaje;
    SerialBT.println(respuesta);
  }
  // Si enviamos algo por el Serial de PC, retransmitirlo por Bluetooth
  if (Serial.available()) {
    char c = Serial.read();
    SerialBT.write(c);
  }
}
