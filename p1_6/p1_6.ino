#include <Arduino_LSM9DS1.h> // Asegúrate de tener esta biblioteca instalada

void setup() {
  Serial.begin(115200);

  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Error: No se pudo inicializar el IMU (LSM9DS1).");
    while (1); // Detener si el IMU no se inicia
  }
}

void loop() {
  float ax, ay, az; // Acelerometro
  float gx, gy, gz; // Giroscopio
  float mx, my, mz; // Magnetómetro


  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);
    Serial.print("A,"); // Prefijo para Acelerometro
    Serial.print(ax, 4); 
    Serial.print(",");
    Serial.print(ay, 4);
    Serial.print(",");
    Serial.println(az, 4);
  }


  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(gx, gy, gz);
    Serial.print("G,"); 
    Serial.print(gx, 4);
    Serial.print(",");
    Serial.print(gy, 4);
    Serial.print(",");
    Serial.println(gz, 4);
  }

  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(mx, my, mz);
    Serial.print("M,"); 
    Serial.print(mx, 4);
    Serial.print(",");
    Serial.print(my, 4);
    Serial.print(",");
    Serial.println(mz, 4);
  }

  delay(100); 
}