#include <Arduino_LSM9DS1.h>  
#include <Wire.h>

#define SLAVE_ADDR 0x08
#define NUM_SAMPLES 5

float ax[NUM_SAMPLES], ay[NUM_SAMPLES], az[NUM_SAMPLES];
float gx[NUM_SAMPLES], gy[NUM_SAMPLES], gz[NUM_SAMPLES];
float mx[NUM_SAMPLES], my[NUM_SAMPLES], mz[NUM_SAMPLES];

void setup() {
    Serial.begin(115200);
    while (!Serial);

    if (!IMU.begin()) {
        Serial.println("No se pudo inicializar el IMU.");
        while (1);
    }

    Wire.begin();
    Serial.println("Master I2C listo.");
}

void loop() {
    captureAndSendData();
    delay(100);
}


//  Captura 5 muestras y llama para enviarlas
void captureAndSendData() {
    Serial.println("Iniciando captura de datos...");

    for (int i = 0; i < NUM_SAMPLES; i++) {
        if (IMU.accelerationAvailable()) IMU.readAcceleration(ax[i], ay[i], az[i]);
        if (IMU.gyroscopeAvailable()) IMU.readGyroscope(gx[i], gy[i], gz[i]);
        if (IMU.magneticFieldAvailable()) IMU.readMagneticField(mx[i], my[i], mz[i]);

        Serial.print("Muestra ");
        Serial.print(i);
        Serial.println(" capturada.");

        if (i < NUM_SAMPLES - 1) {
            delay(200);
        }
    }

    Serial.println("Enviando muestras por I2C...");
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sendFormattedData("A", ax[i], ay[i], az[i]);  
        delay(100);  
        sendFormattedData("G", gx[i], gy[i], gz[i]);
        delay(100);
        sendFormattedData("M", mx[i], my[i], mz[i]);
        delay(100);
    }

    Serial.println("Envío completado.");
}


//  Función para enviar datos
void sendFormattedData(const char* type, float x, float y, float z) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%s%.2f,%.2f,%.2f", type, x, y, z);  
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(buffer);  
    Wire.endTransmission();

    Serial.print("Enviado: ");
    Serial.println(buffer);
}



