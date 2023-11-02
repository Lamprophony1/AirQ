//Include the libraries
#include <MQUnifiedsensor.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

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

// For MQ135
#define MQ135_pin 14
#define MQ135_type "MQ-135"
#define MQ135_ADC_Bit_Resolution 12 
#define MQ135_RatioMQ135CleanAir 3.6

#define DHTPIN 15     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11

uint32_t delayMS = 2000; // Set delay between sensor readings. 2000 ms = 2 seconds.

//Declare Sensor
MQUnifiedsensor MQ5(placa, Voltage_Resolution, MQ5_ADC_Bit_Resolution, MQ5_pin, MQ5_type);
MQUnifiedsensor MQ9(placa, Voltage_Resolution, MQ9_ADC_Bit_Resolution, MQ9_pin, MQ9_type);
MQUnifiedsensor MQ135(placa, Voltage_Resolution, MQ135_ADC_Bit_Resolution, MQ135_pin, MQ135_type);

DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
  //Init the serial port communication - to debug the library
  Serial.begin(115200); //Init serial port
  dht.begin();
  
  //Set math model to calculate the PPM concentration and the value of constants
  //For MQ5
  MQ5.setRegressionMethod(1); 
  MQ5.setA(177.65); MQ5.setB(-2.56);
  MQ5.init(); 
  
  //For MQ9
  MQ9.setRegressionMethod(1); 
  MQ9.setA(599.65); MQ9.setB(-2.244);
  MQ9.init(); 

  //For MQ135
  MQ135.setRegressionMethod(1); 
  MQ135.setA(110.47); MQ135.setB(-2.862); 
  MQ135.init(); 
  
  //Calibrate all sensors
  float MQ5_calcR0 = 0;
  float MQ9_calcR0 = 0;
  float MQ135_calcR0 = 0;
  for(int i = 1; i<=10; i ++) {
    MQ5.update();
    MQ5_calcR0 += MQ5.calibrate(MQ5_RatioMQ5CleanAir);
    MQ9.update();
    MQ9_calcR0 += MQ9.calibrate(MQ9_RatioMQ9CleanAir);
    MQ135.update();
    MQ135_calcR0 += MQ135.calibrate(MQ135_RatioMQ135CleanAir);
    Serial.print(".");
  }
  
  MQ5.setR0(MQ5_calcR0/10);
  MQ9.setR0(MQ9_calcR0/10);
  MQ135.setR0(MQ135_calcR0/10);
  
  Serial.println("  done!.");
  
  //Detect connection issues
  if(isinf(MQ5_calcR0)) {Serial.println("MQ5 Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(MQ5_calcR0 == 0){Serial.println("MQ5 Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  if(isinf(MQ9_calcR0)) {Serial.println("MQ9 Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(MQ9_calcR0 == 0){Serial.println("MQ9 Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  if(isinf(MQ135_calcR0)) {Serial.println("MQ135 Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(MQ135_calcR0 == 0){Serial.println("MQ135 Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  
  MQ5.serialDebug(true);
  MQ9.serialDebug(true);
  MQ135.serialDebug(true);
  Serial.println("     Temperatura| Humedad |  CH4          |      CO     |        CO2 ");
}

void loop() {
  delay(delayMS); //Sampling frequency

  // if you want to apply corelation factor, you will add in this program the temperature and humidity sensor
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float cFactor = 0;
  if (!isnan(event.temperature) && !isnan(event.relative_humidity)) cFactor = getCorrectionFactor(event.temperature, event.relative_humidity);
  
  MQ5.update(); // Update data
  float PPM_MQ5 = MQ5.readSensor(false, cFactor);

  MQ9.update(); // Update data
  float PPM_MQ9 = MQ9.readSensor(false, cFactor);

  MQ135.update(); // Update data
  float PPM_MQ135 = MQ135.readSensor(false, cFactor);

  Serial.printf("%10.2f°C    |%7.2f%% | %7.2f PPM   | %7.2f PPM | %9.2f PPM\n", event.temperature, event.relative_humidity, PPM_MQ5, PPM_MQ9, PPM_MQ135);

  
  delay(10000); //Sampling frequency

}

float getCorrectionFactor(float t, float h) {
  return (0.0405*t) - (0.032 * h) + 0.1775;
}
