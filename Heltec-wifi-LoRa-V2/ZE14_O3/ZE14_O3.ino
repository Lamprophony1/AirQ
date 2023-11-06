// Variables globales
const int analogPinO3 = 38; // Reemplaza 38 con el pin al que está conectado tu sensor de O3
const int maxAnalogReadO3 = 4095; // Máximo valor de lectura analógica en Arduino
const float vRefO3 = 3.3; // Voltaje de referencia en Arduino (3.3V para ESP32, 5V para algunos Arduinos)
const float maxPpmO3 = 10.0; // Máximo valor de concentración de ozono en ppm

// Función para leer el sensor de ozono
float leerSensorO3() {
  int sensorValue = analogRead(analogPinO3);
  float voltage = (sensorValue * vRefO3) / maxAnalogReadO3;
  float ppm = (voltage * maxPpmO3) / vRefO3;
  return ppm;
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  float concentracionO3 = leerSensorO3();
  
  Serial.print("Concentración de O3: ");
  Serial.print(concentracionO3, 2);
  Serial.println(" ppm");
  
  delay(1000);
}
