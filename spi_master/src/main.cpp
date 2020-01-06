#include <Arduino.h>
#include <SPI.h>
#include <BluetoothSerial.h>

#define START_SIGNAL 1
#define TRANSFER_SIGNAL 0xff
#define DATA_SIZE 325
#define CRC16 0x8005
#define BUFFER_SIZE DATA_SIZE + 7
#define CORRECT_CRC 1
#define INCORRECT_CRC 0

bool end_transmission = false;
BluetoothSerial SerialBT;

uint16_t gen_crc16(const uint8_t *data, uint16_t size)
{
    uint16_t out = 0;
    int bits_read = 0, bit_flag;
    if(data == NULL)
        return 0;

    while(size > 0)
    {
        bit_flag = out >> 15;
        out <<= 1;
        out |= (*data >> bits_read) & 1;
        bits_read++;
        if(bits_read > 7)
        {
            bits_read = 0;
            data++;
            size--;
        }
        if(bit_flag)
            out ^= CRC16;

    }
    int i;
    for (i = 0; i < 16; ++i) {
        bit_flag = out >> 15;
        out <<= 1;
        if(bit_flag)
            out ^= CRC16;
    }
    uint16_t crc = 0;
    i = 0x8000;
    int j = 0x0001;
    for (; i != 0; i >>=1, j <<= 1) {
        if (i & out) crc |= j;
    }
    return crc;
}

void setup() {
  Serial.begin(9600);
  SerialBT.begin("bluetooth-blaster-esp32");

  SPI.begin(18,19,23,5);
  SPI.beginTransaction(SPISettings(1000, MSBFIRST, SPI_MODE0));
  SPI.transfer(START_SIGNAL);
  SPI.endTransaction();
}

void loop() {

  if(!end_transmission) {
    SPI.beginTransaction(SPISettings(1000, MSBFIRST, SPI_MODE0));
    uint8_t* buffer = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));
    for(int i = 0; i< BUFFER_SIZE;i++) {
          buffer[i] = (uint8_t) SPI.transfer(TRANSFER_SIGNAL);
          Serial.printf("%c", buffer[i]);
    }
    uint8_t* buffer_cpy = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));
    memcpy(buffer_cpy, buffer, BUFFER_SIZE);
    uint16_t crc = gen_crc16(buffer_cpy, DATA_SIZE+4);
    uint16_t crc_from_buffer = buffer[DATA_SIZE + 5] | (buffer[DATA_SIZE + 5] << 8);
    if(crc == crc_from_buffer) {
      SPI.transfer(CORRECT_CRC);
      if(buffer[0] == 1) {
        end_transmission = true;
      }
      SerialBT.write(buffer, BUFFER_SIZE);
    } else {
      SPI.transfer(INCORRECT_CRC);
    }
    free(buffer);
    free(buffer_cpy);
    SPI.endTransaction();
  }

  //   for (char i = 'a'; i <= 'z'; i++) {
  //   Serial.printf("%c => %d\n", i, (char) SPI.transfer(i));
  // }


}