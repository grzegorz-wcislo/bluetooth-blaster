#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

uint8_t alphabet[26];

void setup() {
  Serial.begin(9600);
  SerialBT.begin("bluetooth-blaster-esp32");

  Serial.write("\n=====\nSetup complete!\n=====\n");

  for (uint8_t i = 'a'; i <= 'z'; i++) {
    alphabet[i - 'a'] = i;
  }

  alphabet[25] = '\0';
}

void loop() {
  // for (char c = 'a'; c <= 'z'; c++) {
  //   SerialBT.write(c);
  // }
  SerialBT.write(alphabet, 25);
}