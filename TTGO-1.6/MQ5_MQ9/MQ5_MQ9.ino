#include <MQUnifiedsensor.h>

//Definitions
#define placa "ESP32"
#define Voltage_Resolution 3.3

// For MQ5
#define MQ5_pin 13
#define MQ5_type "MQ-5"
#define MQ5_ADC_Bit_Resolution 12 
#define MQ5_RatioMQ5CleanAir 6.5

// For MQ9
#define MQ9_pin 12
#define MQ9_type "MQ-9"
#define MQ9_ADC_Bit_Resolution 12 
#define MQ9_RatioMQ9CleanAir 9.6 

//Declare Sensor
MQUnifiedsensor MQ5(placa, Voltage_Resolution, MQ5_ADC_Bit_Resolution, MQ5_pin, MQ5_type);
MQUnifiedsensor MQ9(placa, Voltage_Resolution, MQ9_ADC_Bit_Resolution, MQ9_pin, MQ9_type);

void setup() {
  //Init the serial port communication - to debug the library
  Serial.begin(115200); //Init serial port
  
  //Set math model to calculate the PPM concentration and the value of constants
  //For MQ5
  MQ5.setRegressionMethod(1); 
  MQ5.setA(177.65); MQ5.setB(-2.56);
  MQ5.init(); 
  
  //For MQ9
  MQ9.setRegressionMethod(1); 
  MQ9.setA(599.65); MQ9.setB(-2.244);
  MQ9.init(); 

  //Calibrate both sensors
  float MQ5_calcR0 = 0;
  float MQ9_calcR0 = 0;
  for(int i = 1; i<=10; i ++) {
    MQ5.update();
    MQ5_calcR0 += MQ5.calibrate(MQ5_RatioMQ5CleanAir);
    MQ9.update();
    MQ9_calcR0 += MQ9.calibrate(MQ9_RatioMQ9CleanAir);
    Serial.print(".");
  }
  
  MQ5.setR0(MQ5_calcR0/10);
  MQ9.setR0(MQ9_calcR0/10);
  
  Serial.println("  done!.");
  
  //Detect connection issues for MQ5
  if(isinf(MQ5_calcR0)) {Serial.println("MQ5 Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(MQ5_calcR0 == 0){Serial.println("MQ5 Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  
  //Detect connection issues for MQ9
  if(isinf(MQ9_calcR0)) {Serial.println("MQ9 Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(MQ9_calcR0 == 0){Serial.println("MQ9 Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  
  MQ5.serialDebug(true);
  MQ9.serialDebug(true);
}

void loop() {
  //For MQ5
  MQ5.update();
  MQ5.readSensor();
  Serial.println("Valores del sensor MQ-5:");
  MQ5.serialDebug();

  //For MQ9
  MQ9.update();
  MQ9.readSensor();
  Serial.println("Valores del sensor MQ-9:");
  MQ9.serialDebug();
  
  delay(10000); //Sampling frequency
}
