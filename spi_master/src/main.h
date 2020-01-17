#ifndef main_h
#define main_h

void setup_serial();
void setup_bluetooth();
void setup_spi();

uint8_t* get_buffer_from_SPI();
uint16_t gen_xor(const uint8_t *data, int size);
uint16_t gen_crc16(const uint8_t *data, uint16_t size);
void get_valid_spi_frame();
void get_spi_frame();
bool valid_spi_frame();
void send_bluetooth_buffer();

#endif