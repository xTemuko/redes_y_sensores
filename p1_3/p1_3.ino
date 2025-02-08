#include <Arduino.h>
#include "BBTimer.hpp"

#define ADC_PIN A0
#define PWM_PIN D2
#define VREF 3.3    

BBTimer my_timer(BB_TIMER4);

volatile bool flag = false;
volatile int adcValue = 0;

void timerCallback() {
  flag = true;
}

void setup() {
    Serial.begin(115200);
    pinMode(PWM_PIN, OUTPUT);

    Serial.println("Iniciando temporizador...");
    my_timer.setupTimer(5000000, timerCallback); 
    my_timer.timerStart();
}

void loop() {
    Serial.println("IDLE Task");
    
    if (flag) {  
        flag = false;  
        adcValue = analogRead(ADC_PIN);        
        float voltage = (adcValue * VREF) / 1023.0; 

    
        int dutyCycle = map(adcValue, 0, 1023, 0, 255);
        analogWrite(PWM_PIN, dutyCycle);

        char buffer[50];
        sprintf(buffer, "ADC: %d | Voltaje: %.2f V | PWM: %d", adcValue, voltage, dutyCycle);
        Serial.println(buffer);
    }

    delay(1000);
}
