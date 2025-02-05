#include <Arduino.h>
#include "BBTimer.hpp"  

#define ADC_PIN A6  
#define VREF 3.3    

BBTimer my_timer(BB_TIMER4);

volatile bool flag = false;
volatile int adcValue = 0;

void timerCallback() {
  flag = true;
}

void setup() {
    Serial.begin(115200);

    Serial.println("Iniciando temporizador...");
    my_timer.setupTimer(5000000, timerCallback); 
    my_timer.timerStart();
}

void loop() {

    Serial.println("IDLE Task");
    if (flag) {  // Si la interrupción se activó
        flag = false;  // Resetear flag
        adcValue = analogRead(ADC_PIN);  // Leer ADC en el loop()
        
        float voltage = (adcValue * VREF) / 1023.0; 

        char buffer[50];
        sprintf(buffer, "ADC: %d | Voltaje: %.2f V", adcValue, voltage);
        Serial.println(buffer);
    }
    delay(1000);
}