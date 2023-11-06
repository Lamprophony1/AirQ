#include <SoftwareSerial.h>
#include <ZE07CO_Sensor.h>

#define SensorAnalogPin 37      // Este pin lee el voltaje analógico del sensor de CO
#define VREF 2.1                // Voltaje en el pin AREF

ZE07CO_Sensor ZE07CO(SensorAnalogPin, VREF);

float co_ppm;  // Variable global para almacenar la concentración de CO en ppm

void setup() {
  Serial.begin(115200);
  co_ppm = 0.0;  // Inicialización de la variable global
}

void loop() {
  readCO_Sensor();  // Llamada a la función para leer el sensor
  Serial.print(co_ppm);
  Serial.println(" ppm");
  delay(1000);
}

void readCO_Sensor() {
  co_ppm = ZE07CO.dacReadPPM();  // Leer el sensor y almacenar el valor en la variable global
}
