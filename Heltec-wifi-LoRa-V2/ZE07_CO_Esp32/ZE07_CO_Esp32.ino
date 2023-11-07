#include <SoftwareSerial.h>
#include <ZE07CO_Sensor.h>

#define SensorAnalogPin 37  // Este pin lee el voltaje analógico del sensor de CO
#define VREF 2.1            // Voltaje en el pin AREF

ZE07CO_Sensor ZE07CO(SensorAnalogPin, VREF);

void setup() {
  Serial.begin(115200);
}

void loop() {
  float co_ppm = readCO_Sensor();  // Almacenar el valor retornado por la función en la variable local
  Serial.print(co_ppm);
  Serial.println(" ppm");
  delay(1000);
}

float readCO_Sensor() {
  return ZE07CO.dacReadPPM();  // Leer el sensor y retornar el valor
}
