#include <Wire.h>

#define SLAVE_ADDR 0x08
#define LED_PIN 13

char receivedData[32] = {0};  // Buffer para 32 bytes
bool newData = false;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.begin(115200);
    Wire.begin(SLAVE_ADDR);
    Wire.onReceive(receiveEvent);

    Serial.println("Slave I2C (UNO) listo.");
}

void loop() {
    if (newData) {
        Serial.print("Recibido: ");
        Serial.println(receivedData);

        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);

        newData = false;
    }
}


// Recibir datos por I2C
void receiveEvent(int bytesReceived) {
    memset(receivedData, 0, sizeof(receivedData));  // Limpiar buffer
    int index = 0;

    while (Wire.available() && index < (sizeof(receivedData) - 1)) {
        receivedData[index++] = Wire.read();
    }
    receivedData[index] = '\0';  // Asegurar terminación de cadena

    newData = true;
}

