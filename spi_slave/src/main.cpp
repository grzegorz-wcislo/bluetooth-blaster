#include <mbed.h>

Serial pc(USBTX,USBRX);

SPISlave device(PA_7, PA_6, PA_5, PA_15);

int main() {
  pc.baud(9600);
  device.format(8, 0);
  device.reply(0x00);
  device.frequency(1000);

  char buff[100];
  int i = 0;

  while(1) {
    if (device.receive()) {
      char c = device.read();
      buff[i++] = c;
      // device.reply(c);
      device.reply(0x01);
    }
    if (buff[i] == 'z' || i == 95) {
      buff[i+1] = '\n';
      buff[i+2] = 0;
      pc.printf(buff);
      i = 0;
    }
  }
}