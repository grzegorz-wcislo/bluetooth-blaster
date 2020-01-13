#include <Arduino.h>
#include <SPI.h>
#include <BluetoothSerial.h>

#define START_SIGNAL 0xCC
#define TRANSFER_SIGNAL 0xff
#define DATA_SIZE 330
#define CRC16 0x8005
#define BUFFER_SIZE DATA_SIZE + 7

uint8_t* buffer;


bool end_transmission = false;
BluetoothSerial SerialBT;
uint8_t* get_buffer_from_SPI();
uint16_t gen_crc16(const uint8_t *data, uint16_t size);
void check_and_send_by_bluetooth(int crc, int crc_from_buffer, uint8_t* buffer);
void get_valid_spi_frame();
void get_spi_frame();
bool valid_spi_frame();
void send_bluetooth_buffer();

void setup() {
  Serial.begin(9600);
  SerialBT.begin("bluetooth-blaster-esp32");
  buffer = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));
  SPI.begin(18,19,23,5);
}

void loop() {
  Serial.printf("STARTING READ\n");
  get_valid_spi_frame();
  send_bluetooth_buffer();
  // Serial.printf("%s\n", buffer);
//   if(!end_transmission) {
//     SPI.beginTransaction(SPISettings(1000, MSBFIRST, SPI_MODE0));
//     uint8_t* buffer = get_buffer_from_SPI();
//     uint8_t* buffer_cpy = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));
//     memcpy(buffer_cpy, buffer, BUFFER_SIZE);
//     uint16_t crc = gen_crc16(buffer_cpy, DATA_SIZE+4);
//     uint16_t crc_from_buffer = buffer[DATA_SIZE + 5] | (buffer[DATA_SIZE + 5] << 8);
//     check_and_send_by_bluetooth(crc, crc_from_buffer, buffer);
//     free(buffer);
//     free(buffer_cpy);
//     SPI.endTransaction();
//   }
}

uint8_t* get_buffer_from_SPI() {
  uint8_t* buffer = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));
    for(int i = 0; i< BUFFER_SIZE;i++) {
          buffer[i] = (uint8_t) SPI.transfer(TRANSFER_SIGNAL);
          Serial.printf("%c", buffer[i]);
    }
    return buffer;
}

void check_and_send_by_bluetooth(int crc, int crc_from_buffer, uint8_t* buffer) {
      if(crc == crc_from_buffer) {
      if(buffer[0] == 1) {
        end_transmission = true;
      }
      Serial.printf("%s", "\nCRC CORRECT\n");
      SerialBT.write(buffer, BUFFER_SIZE);
    }
}

void get_valid_spi_frame() {
  do {
    get_spi_frame();
    Serial.printf("\nFRAME GOT\n");
  } while(!valid_spi_frame());
  Serial.printf("\nFRAME VALID\n");
}

void get_spi_frame() {
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(START_SIGNAL);
  for (int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = SPI.transfer(0);
  }
  SPI.endTransaction();
}

bool valid_spi_frame() {
  return true;
}

void send_bluetooth_buffer() {
  SerialBT.write(buffer, BUFFER_SIZE);
  Serial.write(buffer, BUFFER_SIZE);
}

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