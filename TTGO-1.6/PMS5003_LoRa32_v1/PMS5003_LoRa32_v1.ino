 /*
 PM2.5 Demo
  pm25-demo.ino
  Demonstrates operation of PM2.5 Particulate Matter Sensor
  ESP32 Serial Port (RX = 16, TX = 17)
  Derived from howtoelectronics.com - https://how2electronics.com/interfacing-pms5003-air-quality-sensor-arduino/
 
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/
 
// Serial Port connections for PM2.5 Sensor
#include "boards.h"
#include <LoRa.h>
#define RXD2 0// To se--nsor TXD
#define TXD2 2 // To sensor RXD

int counter = 0;
// Variable to keep track of the last time we took a reading
unsigned long lastReadingMillis = 0;
 
void setup() {
  initBoard();
  // When the power is turned on, a delay is required.
    delay(1500);
  // our debugging output
  Serial.begin(115200);
 LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
    if (!LoRa.begin(LoRa_frequency)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
  // Set up UART connection
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  u8g2->setFont(u8g2_font_ncenB08_tr);
}
 
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
 
struct pms5003data data;
 
void loop() {
  // Get the current time
  unsigned long currentMillis = millis();
  // Check if 5 seconds has passed since the last reading (5000 milliseconds)
  if (readPMSdata(&Serial1)) {
    // reading data was successful!
    if(currentMillis - lastReadingMillis >= 5000) {
    Serial.println();
    Serial.println("---------------------------------------");
    Serial.println("Concentration Units (standard)");
    Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);
    Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);
    Serial.print("\t\tPM 10: "); Serial.println(data.pm100_standard);
    Serial.println("---------------------------------------");
    Serial.println("Concentration Units (environmental)");
    Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
    Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
    Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
    Serial.println("---------------------------------------");
    Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(data.particles_03um);
    Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(data.particles_05um);
    Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(data.particles_10um);
    Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(data.particles_25um);
    Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(data.particles_50um);
    Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(data.particles_100um);
    Serial.println("---------------------------------------");

    Serial.print("Sending packet: ");
    Serial.println(counter);

    // send packet
    LoRa.beginPacket();
    LoRa.print("Unid. concentradas: ");
    LoRa.print(data.pm25_standard);
    LoRa.endPacket();

#ifdef HAS_DISPLAY
    if (u8g2) {
        char buf[256];
        char buf0[256];
        char buf1[256];
        u8g2->clearBuffer();
        u8g2->drawStr(0, 10, "Calidad del aire PM2.5");
        snprintf(buf, sizeof(buf), "PM 2.5:  %d", data.pm25_standard);
        u8g2->drawStr(0, 28, buf);
        snprintf(buf0, sizeof(buf0), "PM 10:  %d", data.pm100_standard);
        u8g2->drawStr(0, 46, buf0);
        snprintf(buf1, sizeof(buf1), "PM 1.0:  %d", data.pm10_standard);
        u8g2->drawStr(0, 64, buf1);
        u8g2->sendBuffer();
    }
#endif
    counter++;
    lastReadingMillis = currentMillis;
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
  memcpy((void *)&data, (void *)buffer_u16, 30);
 
  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
 
  // success!
  return true;
}