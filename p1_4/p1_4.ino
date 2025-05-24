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
unsigned int adcInterval = 0; // Intervalo en segundos (0 = desactivado)
float PWMDutyCycle = 0.0;

// Función de interrupción del Timer
void timerCallback() {
    if (adcInterval > 0) {
        flag = true;
    }
}

// Función para leer el ADC y mostrarlo en formato `sprintf`
void readADC() {
    adcValue = analogRead(ADC_PIN);        
    float voltage = (adcValue * VREF) / 1023.0; 

    float PWMDutyCycle = (adcValue/1023.0);
    pwm->write(PWMDutyCycle);

    char buffer[50];
    sprintf(buffer, "ADC: %d | Voltaje: %.2f V | PWM: %.2f", adcValue, voltage, PWMDutyCycle);
    Serial.println(buffer);
}

// Procesa comandos recibidos por UART
void processCommand(char* command) {
    // Comando ADC: Muestra una sola lectura del ADC
    if (strncmp(command, "ADC", 3) == 0 && strlen(command) == 3) {
        readADC();
    }
    // Comando ADC(x): Muestra el ADC cada x segundos (si x=0, detiene)
    else if (strncmp(command, "ADC(", 4) == 0 && command[strlen(command) - 1] == ')') {
        int x = atoi(command + 4);  // Convertir "x" a entero
        if (x >= 0) {
            adcInterval = x;
            my_timer.setupTimer(adcInterval * 1000000, timerCallback);
            if (x == 0) {
                my_timer.timerStop();
            } else {
                my_timer.timerStart();
            }
            char buffer[50];
            sprintf(buffer, "Muestreo ADC cada %d segundos.", x);
            Serial.println(buffer);
        } else {
            Serial.println("Error: Valor inválido en ADC(x).");
        }
    }
    // Comando PWM(x): Configura el duty cycle del PWM (0-9)
    else if (strncmp(command, "PWM(", 4) == 0 && command[strlen(command) - 1] == ')') {
        int x = atoi(command + 4);  // Convertir "x" a entero
        if (x >= 0 && x <= 9) {
            PWMDutyCycle = x / 9.0;
            pwm->write(PWMDutyCycle);

            char buffer[50];
            sprintf(buffer, "PWM duty cycle ajustado a %.2f", PWMDutyCycle);
            Serial.println(buffer);
        } else {
            Serial.println("Error: PWM(x) debe estar entre 0 y 9.");
        }
    } 
    else {
        Serial.println("Comando no reconocido.");
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial); // Esperar la conexión del puerto serie

    // Configurar PWM
    pwm = new mbed::PwmOut(digitalPinToPinName(PWM_PIN));
    pwm->period_us(PWMPeriod);
    
    Serial.println("Sistema iniciado. Esperando comandos...");
}

void loop() {
    if (flag) {  
        flag = false;  
        readADC();  
    }

    // Leer comandos desde el puerto serie
    if (Serial.available()) {
        char command[20];
        int i = 0;

        // Leer hasta encontrar un salto de línea o buffer lleno
        while (Serial.available() && i < sizeof(command) - 1) {
            char c = Serial.read();
            if (c == '\n') break; // Terminar cuando llega un Enter
            command[i++] = c;
        }
        command[i] = '\0';  // Finalizar la cadena

        processCommand(command);
    }
}

