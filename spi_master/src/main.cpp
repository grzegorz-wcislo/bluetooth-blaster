#include <Arduino.h>
#include <SPI.h>

void setup() {
  Serial.begin(9600);

  SPI.begin(18,19,23,5);
}

void loop() {
  SPI.beginTransaction(SPISettings(1000, MSBFIRST, SPI_MODE0));

  for (char i = 'a'; i <= 'z'; i++) {
    Serial.printf("%c => %d\n", i, (char) SPI.transfer(i));
  }

  SPI.endTransaction();
  delay(2000);
}