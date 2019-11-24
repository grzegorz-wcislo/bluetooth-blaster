---
---

# Wstęp

# Środowiska programistyczne

Do dewelopowania używamy wolnego środowiska PlatformIO.

## ESP32

arduino

## STM32 Nucleo

mbedOS

https://os.mbed.com/platforms/ST-Nucleo-l476rg/

# Piny

## ESP32

https://www.cytron.io/image/catalog/products/NODEMCU-ESP32/ESP32-DOIT-DEVKIT-V1-Pinout-30-GPIOs.jpg

## STM32 Nucleo

https://os.mbed.com/platforms/ST-Nucleo-l476rg/
https://www.st.com/content/ccc/resource/technical/document/user_manual/98/2e/fa/4b/e0/82/43/b7/DM00105823.pdf/files/DM00105823.pdf/jcr:content/translations/en.DM00105823.pdf

# GPIO

Do GPIO można wykorzystać dowolne wolne piny GPIO.

# SPI

Wykonałem prosty test przesyłu. Wydaje się być jakiś problem w fazie
wysyłanych bajtów. STM ma problem z odebraniem/wysłaniem danych
poprawnie.

## Piny, STM32 -> ESP32

GND -> GND
SCK/D13 -> D18 (clock)
MISO/D12 -> D19 (miso)
PWM/MOSI/D11 -> D23 (mosi)
A2 -> D5 (slave select)

# Protokół przesyłu GPIO/SPI

# Bluetooth

TODO
