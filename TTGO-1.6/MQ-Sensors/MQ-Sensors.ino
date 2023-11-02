/*
  MQ Gas Sensor Simple test
  mq-simple-test.ino
  Tests operation of MQ-series resistive gas sensors

  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Define analog input pin for sensor (change as required)
#define MQpin (13)

// Variable to store sensor value
float sensorValue;

void setup()
{
  // Set serial port
  Serial.begin(115200);

  delay(1000);

  // Warm up sensor for 10 seconds
  Serial.println("Gas sensor requires warm-up period");
  for (int i = 10; i >= 1; i--) {
    Serial.print("Gas sensor warming up, please wait ");
    Serial.print(i);
    Serial.println(" seconds.");
    delay(1000);
  }

}

void loop()
{

  // Read sensor pin value
  sensorValue = analogRead(MQpin);

  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  // Delay between readings
  delay(2000);

}