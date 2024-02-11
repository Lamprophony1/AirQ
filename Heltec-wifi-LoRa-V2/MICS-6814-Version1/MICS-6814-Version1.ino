#include "MICS6814.h"


#define PIN_NO2 A4


MICS6814 gas(PIN_NO2);

void setup() {
  Serial.begin(9600);

  Serial.println("MICS-6814 Sensor Sample");
  Serial.print("Calibrating Sensor");

  gas.calibrate();

  Serial.println("OK!");
}

void loop() {
  Serial.print("NO2: ");
  Serial.print(gas.getResistance(CH_NO2));
  Serial.print("/");
  Serial.print(gas.getBaseResistance(CH_NO2));
  Serial.print(" = ");
  Serial.print(gas.getCurrentRatio(CH_NO2));
  Serial.print(" => ");
  Serial.print(gas.measure(NO2));
  Serial.println("ppm");
  delay(50);

  delay(1000);
}
