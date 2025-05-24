#include <Wire.h>

#define SLAVE_ADDR 0x08  // Dirección I2C del esclavo
#define LED_PIN D2       // Pin a controlar

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Wire.begin(SLAVE_ADDR);
    Wire.onReceive(receiveEvent);

    Serial.begin(115200);
    Serial.println("Arduino Nano 33 BLE Slave I2C listo.");
}

void receiveEvent(int bytes) {
    // Se espera recibir un solo byte
    if (Wire.available() > 0) {
        uint8_t cmd = Wire.read();  // Leer el byte recibido
        Serial.print("Recibido: ");
        Serial.println(cmd);

        if (cmd == 1) {
            digitalWrite(LED_PIN, HIGH);
            Serial.println("LED ENCENDIDO");
        }
        else if (cmd == 0) {
            digitalWrite(LED_PIN, LOW);
            Serial.println("LED APAGADO");
        }
        else {
            Serial.println("Comando no reconocido.");
        }
    }
}

void loop() {
    // No es necesario hacer nada en loop(), ya que se usa la interrupción I2C.
}




