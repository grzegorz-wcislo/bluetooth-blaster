#include <mbed.h>

#define DATA_SIZE 330
#define CRC16 0x8005
#define BUFFER_SIZE DATA_SIZE + 2
#define CORRECT_CRC 1
#define START_SIGNAL 0xCC
#define SPI_FREQUENCY 1000

Serial pc(USBTX,USBRX);

SPISlave device(PA_7, PA_6, PA_5, PA_15);

int send_packet(uint8_t* buffer);

void generate_random_data(uint8_t* buffer);

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

int main() {
  device.frequency(SPI_FREQUENCY);
  uint8_t* buffer = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));

  generate_random_data(buffer);

  while (true) {
    device.reply(0xCC);
    if (device.receive()) {
      if (device.read() == START_SIGNAL) {
        while(send_packet(buffer) != 0);
        generate_random_data(buffer);
      }
    }
  }
  // srand(time(NULL));
  // pc.baud(9600);
  // device.format(8, 0);
  // device.reply(0x00);
  // device.frequency(1000);

  // bool start_command_recieved = false;
  // uint32_t frame_counter = 0;
  // pc.printf("WAITING FOR A DEVICE\n");
  // while(1) {
  //   if(device.receive()) {
  //     pc.printf("STARTING TO SEND DATA\n");
  //     start_command_recieved = true;
  //   }

  //   if(start_command_recieved) {
  //     for(int j = 0;j<200;j++) {
  //        uint8_t* buffer = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));
  //        if(j == 199) {
  //         buffer[0] = (uint8_t) 1;
  //        } else {
  //          buffer[0] = (uint8_t) 0;
  //        }
  //       for(int i = 1; i <= DATA_SIZE; i++) {
  //         buffer[i] = (uint8_t) (rand()%26 + 'a');
  //       }
  //       buffer[DATA_SIZE + 1] = frame_counter;
  //       buffer[DATA_SIZE + 2] = (frame_counter >> 8);
  //       buffer[DATA_SIZE + 3] = (frame_counter >> 16);
  //       buffer[DATA_SIZE + 4] = (frame_counter >> 24);
  //       frame_counter++;
  //       uint8_t* buffer_cpy = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));
  //       memcpy(buffer_cpy, buffer, BUFFER_SIZE);
  //       uint16_t crc = gen_crc16(buffer_cpy, DATA_SIZE+4);
  //       buffer[DATA_SIZE + 5] = crc;
  //       buffer[DATA_SIZE + 6] = (crc >> 8);
  //       pc.printf("IS LAST FRAME: ");
  //       pc.printf("%d", buffer[0]);
  //       pc.printf("\nDATA SENT: ");
  //       for(int i = 1; i <= DATA_SIZE;i++) {
  //         pc.printf("%c", buffer[i]);
  //       }
  //         for(int i = 0; i< BUFFER_SIZE; i++) {
  //           device.reply((int) buffer[i]);
  //         }
  //       free(buffer);
  //       free(buffer_cpy);
  //     }
  //   }

  // }

//  char buff[100];
//   int i = 0;

  // while(1) {
  //   if (device.receive()) {
  //     char c = device.read();
  //     buff[i++] = c;
  //     // device.reply(c);
  //     device.reply(0x01);
  //   }
  //   if (buff[i] == 'z' || i == 95) {
  //     buff[i+1] = '\n';
  //     buff[i+2] = 0;
  //     pc.printf(buff);
  //     i = 0;
  //   }
  // }
}

void generate_random_data(uint8_t* buffer) {
  for(int i = 0; i < DATA_SIZE; i++) {
    buffer[i] = (uint8_t) (rand()%26 + 'a');
  }
  uint8_t* buffer_cpy = (uint8_t *) malloc(BUFFER_SIZE * sizeof(uint8_t));
  memcpy(buffer_cpy, buffer, BUFFER_SIZE);
  uint16_t crc = gen_xor(buffer_cpy, DATA_SIZE);
  buffer[DATA_SIZE] = crc;
  buffer[DATA_SIZE + 1] = (crc >> 8);
  //pc.printf("%d %d\n", buffer[DATA_SIZE], buffer[DATA_SIZE+1]);
  //   buffer[DATA_SIZE] = 'X';
  // buffer[DATA_SIZE + 1] = 'X';
}

int send_packet(uint8_t* buffer) {
  for(int i = 0;i<BUFFER_SIZE;i++) {
    device.reply(buffer[i]);
    // if (device.read() == START_SIGNAL) {
    //   //pc.printf("DUPA\n");
    //   return 1;
    // }
  }
  return 0;
}