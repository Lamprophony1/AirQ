#include "LoRaWan_APP.h"
#include "DHTesp.h"
#include "Arduino.h"
#include <SoftwareSerial.h>
#include <ZE07CO_Sensor.h>

//DHT11 (temp&hum)
int pinDHT = 4;
DHTesp dht;

//PMS5003(Material particulado)
#define RXD2 23// To se--nsor TXD
#define TXD2 17 // To sensor RXD

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
struct pms5003data pmdata;

//MICS-6814 (Dioxido de nitrogeno (NO2))
const int S_analog = 1023;
const int NO2_pin = 36; // Pin para el sensor de Dióxido de Nitrógeno

float leerNO2() {
  int valorSensor = analogRead(NO2_pin);
  float no2 = map(valorSensor, 0, S_analog, 3.3, 1000) / 100.0;
  return no2; // Ya está en ppm
}

//ZE07-CO (Monóxido de carbono)
#define SensorAnalogPin 37      // Este pin lee el voltaje analógico del sensor de CO
#define VREF 2.1                // Voltaje en el pin AREF
ZE07CO_Sensor ZE07CO(SensorAnalogPin, VREF);


float readCO_Sensor() {
  return ZE07CO.dacReadPPM();  // Leer el sensor y retornar el valor
}

//ZE14-O3 (Ozono)
const int analogPinO3 = 38; // Reemplaza 38 con el pin al que está conectado tu sensor de O3
const int maxAnalogReadO3 = 4095; // Máximo valor de lectura analógica en Arduino
const float vRefO3 = 3.3; // Voltaje de referencia en Arduino (3.3V para ESP32, 5V para algunos Arduinos)
const float maxPpmO3 = 10.0; // Máximo valor de concentración de ozono en ppm

float leerSensorO3() {
  int sensorValue = analogRead(analogPinO3);
  float voltage = (sensorValue * vRefO3) / maxAnalogReadO3;
  float ppm = (voltage * maxPpmO3) / vRefO3;
  return ppm;
}


/* OTAA para*/
uint8_t devEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0x24, 0xD7 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x4D, 0x3D, 0xC5, 0x3F, 0xDB, 0xCC, 0x35, 0xF7, 0xD7, 0xDF, 0x51, 0xBE, 0xA8, 0xA7, 0x23, 0x65 };

/* ABP para*/
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda,0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef,0x67 };
uint32_t devAddr =  ( uint32_t )0x007e6ae1;

/*LoraWan channelsmask*/
uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = CLASS_A;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 10000;

/*OTAA or ABP*/
bool overTheAirActivation = true;

/*ADR enable*/
bool loraWanAdr = true;


/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = true;

/* Application port */
uint8_t appPort = 2;
/*!
* Number of trials to transmit the frame, if the LoRaMAC layer did not
* receive an acknowledgment. The MAC performs a datarate adaptation,
* according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
* to the following table:
*
* Transmission nb | Data Rate
* ----------------|-----------
* 1 (first)       | DR
* 2               | DR
* 3               | max(DR-1,0)
* 4               | max(DR-1,0)
* 5               | max(DR-2,0)
* 6               | max(DR-2,0)
* 7               | max(DR-3,0)
* 8               | max(DR-3,0)
*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;


RTC_DATA_ATTR bool firstrun = true;

void setup() {
	Serial.begin(115200);
  Mcu.begin();
  if(firstrun)
  {
    LoRaWAN.displayMcuInit();
    firstrun = false;
  }
	deviceState = DEVICE_STATE_INIT;
  userChannelsMask[0] = (1 << 8) | (1 << 9);
  dht.setup(pinDHT, DHTesp::DHT22);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(1000);  // Espera de 1 segundo al inicio
}

void loop()
{
  if (readPMSdata(&Serial1)) {
  }

	switch( deviceState )
	{
		case DEVICE_STATE_INIT:
		{
#if(LORAWAN_DEVEUI_AUTO)
			LoRaWAN.generateDeveuiByChipID();
#endif
			LoRaWAN.init(loraWanClass,loraWanRegion);
			break;
		}
		case DEVICE_STATE_JOIN:
		{
      LoRaWAN.displayJoining();
			LoRaWAN.join();
			break;
		}
		case DEVICE_STATE_SEND:
		{
      LoRaWAN.displaySending();
			prepareTxFrame( appPort );
			LoRaWAN.send();
			deviceState = DEVICE_STATE_CYCLE;
			break;
		}
		case DEVICE_STATE_CYCLE:
		{
			// Schedule next packet transmission
			txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
			LoRaWAN.cycle(txDutyCycleTime);
			deviceState = DEVICE_STATE_SLEEP;
			break;
		}
		case DEVICE_STATE_SLEEP:
		{
      LoRaWAN.displayAck();
			LoRaWAN.sleep(loraWanClass);
			break;
		}
		default:
		{
			deviceState = DEVICE_STATE_INIT;
			break;
		}
	}
}


boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
 
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }
 
  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
 
  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  // get checksum ready
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }
 
  /* debugging
    for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
    }
    Serial.println();
  */
 
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }
 
  // put it into a nice struct :)
  memcpy((void *)&pmdata, (void *)buffer_u16, 30);
 
  if (sum != pmdata.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
 
  // success!
  return true;
}

/* Prepares the payload of the frame */
static void prepareTxFrame(uint8_t port) {
    TempAndHumidity data = dht.getTempAndHumidity();
    float co = readCO_Sensor();  // Leer Monóxido de Carbono
    float no2 = leerNO2();       // Leer Dióxido de Nitrógeno
    float o3 = leerSensorO3();   // Leer Ozono

    // Convertir los valores de temperatura y humedad a enteros, por ejemplo multiplicando por 10
    int16_t temperature = (int16_t)(data.temperature * 10);
    int16_t humidity = (int16_t)(data.humidity * 10);

    // Convertir los valores de CO, NO2 y O3 a enteros, por ejemplo multiplicando por 100
    int16_t co_int = (int16_t)(co * 100);
    int16_t no2_int = (int16_t)(no2 * 100);
    int16_t o3_int = (int16_t)(o3 * 100);

    // Obtener valores de PM10 y PM2.5
    int16_t pm10 = (int16_t)pmdata.pm10_standard;
    int16_t pm25 = (int16_t)pmdata.pm25_standard;

    appDataSize = 14; // Actualizar tamaño del payload (7 valores de 16 bits cada uno)
    // Preparar el paquete de datos
    appData[0] = temperature >> 8; // Byte alto de temperatura
    appData[1] = temperature & 0xFF; // Byte bajo de temperatura
    appData[2] = humidity >> 8; // Byte alto de humedad
    appData[3] = humidity & 0xFF; // Byte bajo de humedad
    appData[4] = pm10 >> 8; // Byte alto de PM10
    appData[5] = pm10 & 0xFF; // Byte bajo de PM10
    appData[6] = pm25 >> 8; // Byte alto de PM2.5
    appData[7] = pm25 & 0xFF; // Byte bajo de PM2.5
    appData[8] = co_int >> 8; // Byte alto de CO
    appData[9] = co_int & 0xFF; // Byte bajo de CO
    appData[10] = no2_int >> 8; // Byte alto de NO2
    appData[11] = no2_int & 0xFF; // Byte bajo de NO2
    appData[12] = o3_int >> 8; // Byte alto de O3
    appData[13] = o3_int & 0xFF; // Byte bajo de O3
}
