#include "main.h"

#define START_SIGNAL 0xCC
#define TRANSFER_SIGNAL 0xff
#define DATA_SIZE 330
#define CRC16 0x8005
#define BUFFER_SIZE DATA_SIZE + 1
#define SPI_FREQUENCY 1000000

BluetoothSerial SerialBT;

spi_device_handle_t device_handle;
spi_transaction_t transaction;
uint8_t txbuffer[BUFFER_SIZE];
uint8_t rxbuffer[BUFFER_SIZE];

void setup() {
  setup_serial();
  setup_bluetooth();
  setup_spi();
}

void loop() {
  get_valid_spi_frame();
  send_bluetooth_buffer();
}

void get_valid_spi_frame() {
  do {
    get_spi_frame();
  } while(!valid_spi_frame());
}

void get_spi_frame() {
  spi_device_transmit(device_handle, &transaction);
}

bool valid_spi_frame() {
  // uint8_t* buffer_cpy = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));
  // memcpy(buffer_cpy, buffer, BUFFER_SIZE);
  // uint16_t crc = gen_xor(buffer_cpy, DATA_SIZE);
  // uint16_t crc_from_buffer = buffer[DATA_SIZE] | (buffer[DATA_SIZE+1]<<8);
  // return crc_from_buffer == crc;
  return true;
}

void send_bluetooth_buffer() {
  SerialBT.write(rxbuffer+1, DATA_SIZE);
}

uint16_t gen_xor(const uint8_t *data, int size) {
  uint16_t out = 0;
  for (int i = 0; i < size; i++) {
    out ^= data[i];
  }
  return out;
}

void setup_serial() {
  Serial.begin(9600);
}

void setup_bluetooth() {
  SerialBT.begin("bluetooth-blaster-esp32");
}

void setup_spi() {
  txbuffer[0] = START_SIGNAL;

  transaction = {
    0, //flags
    0, //cmd
    0, //addr
    BUFFER_SIZE*8, //length
    0, //rxlength
    0, //user
    &txbuffer, //tx_buffer
    &rxbuffer //rx_buffer
  };

  spi_bus_config_t bus_config = {
    .mosi_io_num = 23,
    .miso_io_num = 19,
    .sclk_io_num = 18,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 0,
    .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_NATIVE_PINS,
    .intr_flags = 0
  };

  spi_device_interface_config_t device_config = {
    .command_bits = 0,
    .address_bits = 0,
    .dummy_bits = 0,
    .mode = 0,
    .duty_cycle_pos = 0,
    .cs_ena_pretrans = 0,
    .cs_ena_posttrans = 0,
    .clock_speed_hz = SPI_FREQUENCY,
    .input_delay_ns = 0,
    .spics_io_num = 5,
    .flags = 0,
    .queue_size = 1,
    .pre_cb = 0,
    .post_cb = 0,
  };

  Serial.printf("%d\n", spi_bus_initialize(VSPI_HOST, &bus_config, 1));
  Serial.printf("%d\n", spi_bus_add_device(VSPI_HOST, &device_config, &device_handle));
}