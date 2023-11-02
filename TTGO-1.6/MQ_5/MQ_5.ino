#include <MQUnifiedsensor.h>

#define placa "ESP32"
#define Voltage_Resolution 3.3
#define pin 13 //Analog input 0 of your arduino
#define type "MQ-5" //MQ5
#define ADC_Bit_Resolution 12 // For arduino UNO/MEGA/NANO
#define RatioMQ5CleanAir 6.5  //RS / R0 = 6.5 ppm 

MQUnifiedsensor MQ5(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

unsigned long previousMillis = 0; 
const long interval = 500; // intervalo para mostrar lecturas (en milisegundos)

void setup() {
  Serial.begin(115200);
  MQ5.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ5.setA(177.65); MQ5.setB(-2.56);
  MQ5.init();  

  Serial.print("Calibrando por favor espere.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++) {
    MQ5.update();
    calcR0 += MQ5.calibrate(RatioMQ5CleanAir);
    Serial.print(".");
  }
  MQ5.setR0(calcR0/10);
  Serial.println("  Listo!.");

  if(isinf(calcR0)) {Serial.println("Advertencia: Problema de conexión, R0 es infinito (circuito abierto detectado) por favor verifique su cableado y alimentación"); while(1);}
  if(calcR0 == 0){Serial.println("Advertencia: Se ha encontrado un problema de conexión, R0 es cero (El pin analógico está en corto con tierra) por favor verifique su cableado y alimentación"); while(1);}

  MQ5.serialDebug(false);
}

void loop() {
  MQ5.update(); // Update data, the arduino will read the voltage from the analog pin
  MQ5.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Solo muestra los valores de PPM
    Serial.print("Concentración de metano (CH4) en PPM: ");
    Serial.println(MQ5.readSensor());
  }
}
