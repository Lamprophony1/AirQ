#include "arduino.h"

// PM2.5 Sensor Definitions
#define RXD2 0 // To sensor TXD
#define TXD2 2 // To sensor RXD
unsigned long lastReadingMillis = 0;

void setup() {
  // PM2.5 Setup
  delay(1500);
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
} pmData;

void loop() {
  // PM2.5 Loop
  unsigned long currentMillis = millis();
  if (readPMSdata(&Serial1)) {
    if(currentMillis - lastReadingMillis >= 10000) {
      lastReadingMillis = currentMillis;
      // Print PM2.5 sensor readings
      Serial.print("PM 1.0 (ug/m3): "); Serial.println(pmData.pm10_standard);
      Serial.print("PM 2.5 (ug/m3): "); Serial.println(pmData.pm25_standard);
      Serial.print("PM 10.0 (ug/m3): "); Serial.println(pmData.pm100_standard);
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
 
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }
 
  // put it into a nice struct :)
  memcpy((void *)&pmData, (void *)buffer_u16, 30);
 
  if (sum != pmData.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
 
  // success!
  return true;
}
