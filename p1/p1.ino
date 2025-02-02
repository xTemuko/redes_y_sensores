#define ADC_PIN A6  
#define VREF 3.3    

void setup() {
    Serial.begin(115200);  
}

void loop() {

    int adcValue = analogRead(ADC_PIN); 
    float voltage = (adcValue * VREF) / 1023.0;  

    char buffer[50];
    sprintf(buffer, "ADC: %d | Voltaje: %.2f V", adcValue, voltage);
    Serial.println(buffer);  

    delay(1000);  
}
