#include <Arduino.h>
#include "BBTimer.hpp"  

#define ADC_PIN A6  
#define VREF 3.3    

BBTimer my_timer(BB_TIMER0);  

void timerCallback() {
    int adcValue = analogRead(ADC_PIN);
    float voltage = (adcValue * VREF) / 1023.0; 

    char buffer[50];
    sprintf(buffer, "ADC: %d | Voltaje: %.2f V", adcValue, voltage);
    Serial.println(buffer);
}

void setup() {
    Serial.begin(115200);
    my_timer.setupTimer(5000000, timerCallback); 
    my_timer.timerStart();
}

void loop() {
    Serial.println("IDLE Task");
    delay(1000);
}
