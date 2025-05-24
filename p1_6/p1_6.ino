#include <Arduino_LSM9DS1.h> 
#define NUM_SAMPLES 10

// Arreglos para almacenar las lecturas
float ax[NUM_SAMPLES], ay[NUM_SAMPLES], az[NUM_SAMPLES];  // Acelerómetro
float gx[NUM_SAMPLES], gy[NUM_SAMPLES], gz[NUM_SAMPLES];  // Giroscopio
float mx[NUM_SAMPLES], my[NUM_SAMPLES], mz[NUM_SAMPLES];  // Magnetómetro

int sampleIndex = 0;  

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("No se pudo inicializar el IMU (LSM9DS1).");
    while (1); 
  }
  Serial.println("IMU inicializado correctamente.");

}

void loop() {

  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;
  float xMag, yMag, zMag;


  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(xAcc, yAcc, zAcc);
  }
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(xGyro, yGyro, zGyro);
  }
  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(xMag, yMag, zMag);
  }

  ax[sampleIndex] = xAcc;
  ay[sampleIndex] = yAcc;
  az[sampleIndex] = zAcc;

  gx[sampleIndex] = xGyro;
  gy[sampleIndex] = yGyro;
  gz[sampleIndex] = zGyro;

  mx[sampleIndex] = xMag;
  my[sampleIndex] = yMag;
  mz[sampleIndex] = zMag;

  sampleIndex++;

  if (sampleIndex == NUM_SAMPLES) {
    Serial.println("================= ENVÍO DE BLOQUE =================");
    for (int i = 0; i < NUM_SAMPLES; i++) {
      Serial.print("Muestra ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print("ACC(");
      Serial.print(ax[i], 2); Serial.print(", ");
      Serial.print(ay[i], 2); Serial.print(", ");
      Serial.print(az[i], 2); Serial.print(") | ");

      Serial.print("GYR(");
      Serial.print(gx[i], 2); Serial.print(", ");
      Serial.print(gy[i], 2); Serial.print(", ");
      Serial.print(gz[i], 2); Serial.print(") | ");

      Serial.print("MAG(");
      Serial.print(mx[i], 2); Serial.print(", ");
      Serial.print(my[i], 2); Serial.print(", ");
      Serial.print(mz[i], 2); Serial.println(")");
    }
    Serial.println("====================================================\n");

    sampleIndex = 0;
  }

  delay(100);
}
