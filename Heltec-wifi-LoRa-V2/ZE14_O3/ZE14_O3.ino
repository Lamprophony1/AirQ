#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, -1); // RX en pin 10, TX no se usa

void setup() {
  Serial.begin(9600); // Inicia la comunicación serial con la PC
  mySerial.begin(9600); // Inicia la comunicación serial con el sensor
  Serial.println("Iniciando sensor ZE14-O3...");
}

void loop() {
  if (mySerial.available()) {
    byte data[9]; // Asumiendo que el sensor envía 9 bytes por lectura
    for (int i = 0; i < 9; i++) { // Lee los 9 bytes
      while (!mySerial.available()); // Espera a que esté disponible el siguiente byte
      data[i] = mySerial.read(); // Lee el byte
    }

    // Verifica el byte de inicio y el tipo de gas aquí, por ejemplo:
    if (data[0] == 0xFF && data[1] == 0x2A) { // Asumiendo 0xFF como byte de inicio y 0x2A como identificador de ozono
      // Asumiendo que los bytes 4 y 5 contienen la información de la concentración en ppm
      int concentration = data[4] * 256 + data[5]; // Convierte los bytes de concentración
      
      Serial.print("Concentración de Ozono: ");
      Serial.print(concentration);
      Serial.println(" ppm");
      
      // Opcional: Verifica el checksum aquí, si es necesario
    }
  }
  delay(1000); // Espera un poco antes de la próxima lectura
}
