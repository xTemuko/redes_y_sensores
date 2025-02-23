#include <Wire.h>

#define SLAVE_ADDR 0x08  // Dirección I2C del esclavo

void setup() {
    Serial.begin(115200);
    Wire.begin();  // Iniciar I2C como Master
    Serial.println("Arduino UNO Master I2C listo.");
}

void loop() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();  // Elimina espacios

        if (command == "0" || command == "1") {
            int value = command.toInt();
            sendCommand(value);
        } 
        else {
            Serial.println("Comando no reconocido.");
        }
    }
}

// Función para enviar datos al Slave
void sendCommand(uint8_t cmd) {
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(cmd);  // Enviar cadena
    Wire.endTransmission();
    
    Serial.print("Comando enviado: ");
    Serial.println(cmd);
}

