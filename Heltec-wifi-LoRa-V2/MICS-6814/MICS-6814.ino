const int S_analog = 1023;
const int NO2_pin = 36; // Pin para el sensor de Dióxido de Nitrógeno

// Funciones para leer los sensores

float leerNO2() {
  int valorSensor = analogRead(NO2_pin);
  float no2 = map(valorSensor, 0, S_analog, 3.3, 1000) / 100.0;
  return no2; // Ya está en ppm
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  float no2 = leerNO2();
  
  Serial.print("NO2: ");
  Serial.print(no2, 2);
  Serial.println(" ppm");

  delay(1000);
}
