#include <Arduino.h>
#include "BBTimer.hpp"
#include "mbed.h"

#define ADC_PIN A0
#define PWM_PIN D2
#define VREF 3.3  

const unsigned int PWMPeriod = 200; // 200us - 5kHz  
mbed::PwmOut* pwm;

BBTimer my_timer(BB_TIMER4);

volatile bool flag = false;
volatile int adcValue = 0;

void timerCallback() {
  flag = true;
}

void setup() {
    Serial.begin(115200);

    // set up the pwm object for PWMPin
    pwm = new mbed::PwmOut(digitalPinToPinName(PWM_PIN));

    // set the PWM period to PWMPeriod
    pwm->period_us(PWMPeriod);
    

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

    
        float PWMDutyCycle = (adcValue/1023.0);
        pwm->write(PWMDutyCycle);

        char buffer[50];
        sprintf(buffer, "ADC: %d | Voltaje: %.2f V | PWM: %.2f", adcValue, voltage, PWMDutyCycle);
        Serial.println(buffer);
    }

    delay(1000);
}
