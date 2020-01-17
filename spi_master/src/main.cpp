#include <Arduino.h>
#include <SPI.h>
#include <BluetoothSerial.h>
#include <driver/spi_master.h>
//#include "spi_master.h"

#define START_SIGNAL 0xCC
#define TRANSFER_SIGNAL 0xff
#define DATA_SIZE 330
#define CRC16 0x8005
#define BUFFER_SIZE DATA_SIZE + 2
#define SPI_FREQUENCY 500000

uint8_t* buffer;

uint8_t txbuffer[BUFFER_SIZE + 1];
uint8_t rxbuffer[BUFFER_SIZE + 1];


bool end_transmission = false;
spi_device_handle_t device_handle;
BluetoothSerial SerialBT;
uint8_t* get_buffer_from_SPI();
uint16_t gen_xor(const uint8_t *data, int size);
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
  // SPI.begin(18,19,23,5);
  txbuffer[0] = START_SIGNAL;
  spi_bus_config_t bus_config = {
    23,
    19,
    18,
    -1,
    -1,
    0,
    SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_NATIVE_PINS,
    0
  };
  spi_device_interface_config_t device_config = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    SPI_FREQUENCY,
    0,
    5,
    0,
    1,
    0,
    0
  };
  Serial.printf("%d\n", spi_bus_initialize(VSPI_HOST, &bus_config, 1));
  Serial.printf("%d\n", spi_bus_add_device(VSPI_HOST, &device_config, &device_handle));
  }

void loop() {
  //Serial.printf("STARTING READ\n");
  get_valid_spi_frame();
  // Serial.printf("%s", rxbuffer+1);
  // for (int i = 0; i < BUFFER_SIZE + 1; i++) {
  //   Serial.printf("%x|", rxbuffer[i]);
  // }
  // Serial.printf("\n");
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
//delay(1000);
}

void get_valid_spi_frame() {
  // do {
    get_spi_frame();
    //Serial.printf("\nFRAME GOT\n");
  // } while(!valid_spi_frame());
  //Serial.printf("\nFRAME VALID\n");
}

void get_spi_frame() {
  // SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
  // SPI.transfer(START_SIGNAL);
    // buffer[i] = SPI.transfer(0);
    spi_transaction_t transaction = {
      0,
      0,
      0,
      (BUFFER_SIZE+1)*8,
      0,
      0,
      &txbuffer,
      &rxbuffer,
    };
    spi_device_transmit(device_handle, &transaction);
  // SPI.endTransaction();
}

bool valid_spi_frame() {
  uint8_t* buffer_cpy = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));
  memcpy(buffer_cpy, buffer, BUFFER_SIZE);
  uint16_t crc = gen_xor(buffer_cpy, DATA_SIZE);
  uint16_t crc_from_buffer = buffer[DATA_SIZE] | (buffer[DATA_SIZE+1]<<8);
  return crc_from_buffer == crc;
  // Serial.printf("%d - %d", crc, crc_from_buffer);
  return true;
}

void send_bluetooth_buffer() {
  SerialBT.write(rxbuffer+1, BUFFER_SIZE);
  //Serial.write(buffer, BUFFER_SIZE);
  //Serial.printf("\n");
}

uint16_t gen_xor(const uint8_t *data, int size) {
  uint16_t out = 0;
  for (int i = 0; i < size; i++) {
    out ^= data[i];
  }
  return out;
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