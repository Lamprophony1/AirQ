#include <MQUnifiedsensor.h>

#define Board "ESP32"
#define Pin 35 //Analog input 4 of your arduino
#define Type "MQ-9" //MQ9
#define Voltage_Resolution 3.3
#define ADC_Bit_Resolution 12 // For arduino UNO/MEGA/NANO
#define RatioMQ9CleanAir 9.6 //RS / R0 = 60 ppm 

//Declare Sensor
MQUnifiedsensor MQ9(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

// Variables para controlar el tiempo sin usar delay()
unsigned long previousMillis = 0; 
const long interval = 500; // intervalo en el cual quieres mostrar la información (5000ms = 5s)

void setup() {
  //Init the serial port communication - to debug the library
  Serial.begin(115200); //Init serial port

  //Set math model to calculate the PPM concentration and the value of constants
  MQ9.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ9.setA(599.65); MQ9.setB(-2.244); // Configure the equation to to calculate LPG concentration
  
  MQ9.init(); 

  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ9.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ9.calibrate(RatioMQ9CleanAir);
    Serial.print(".");
  }
  MQ9.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  
  MQ9.serialDebug(true);
}

void loop() {
  
  MQ9.update(); // Update data, the arduino will read the voltage from the analog pin

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    MQ9.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

    // Imprimir solo el valor de PPM de Monóxido de Carbono
    Serial.print("La concentración actual de Monóxido de Carbono (CO) en el ambiente es de: ");
    Serial.print(MQ9.readSensor());
    Serial.println(" partes por millon (PPM)");
  }
}
