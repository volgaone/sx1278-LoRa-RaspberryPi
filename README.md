# sx1278-LoRa-RaspberryPi
## Library to work with sx1278 LoRa chips on Raspberry pi
### It's just for P2P transmissions. Not for LoRaWAN.

May work with all sx127x chips, I didn't test it on these chips


This has been tested on Raspberry Pi 5 hardware. It will not work on earlier versions of RPI hardware,
because previous versions use a different hardware chip, and require `libpigpio` library. On the other hand,
RPI5 uses `gpiod` library for simple IO manipulation, and `wiringPiSPI` library for SPI transfers.
If you'd like to test this code on earlier versions of RPI (3 & 4), go to this [repo](https://github.com/YandievRuslan/sx1278-LoRa-RaspberryPi) 

Quick start:

1. Wire a Raspberry Pi 5 and LoRa chip according to the table below

| raspi                   |   | LoRa  |
|-------------------------|---|-------|
| GPIO4, pin 7            |   | RESET |
| GPIO17, pin 11          |   | DIO0  |
| MOSI (GPIO10, pin 19)   |   | MOSI  |
| MISO (GPIO9, pin 21)    |   | MISO  |
| CLK (GPIO11, pin 23)    |   | SCK   |
| SPI_CE0 (GPIO8, pin 24) |   | NSS   |

2. Clone the repo
3. Enter cloned repo dir
4. `make`
5. Try files under sudo: `transmit_explicit`, `transmit_implicit`, `receive_explicit`, `receive_implicit`
All transmit examples just ones send "LoRa" string with its '\0' terminating byte.

### API:

- `int LoRa_begin(LoRa_ctl *modem)` - Configures radio module. Returns SPI handle= 0 if OK, otherwise error number 0
- `void LoRa_send(LoRa_ctl *modem)` -Commands radio module to send data from buffer
- `void LoRa_receive(LoRa_ctl *modem)` -Commands radio module to switch to continuous receive mode
- `void LoRa_calculate_packet_t(LoRa_ctl *modem)` - Calculates on-air time, payload symbols and other fields.
- `_Bool LoRa_check_conn(LoRa_ctl *modem)` -Checks connection with module. Return True/False
- `void LoRa_end(LoRa_ctl *modem)` - Ends up working with module. Switches to sleep mode, closes SPI and so on.
- `void LoRa_stop_receive(LoRa_ctl *modem)` - Removes callback from IRQ pin, switches module to idle mode.
- `void LoRa_sleep(LoRa_ctl *modem)` - Switches module to sleep mode
- `unsigned char LoRa_get_op_mode(LoRa_ctl *modem)` Returns current module operational mode - SLEEP_MODE, RXCONT_MODE, SLEEP_MODE, STDBY_MODE


### Notice:

Function void LoRa_receive(LoRa_ctl *modem) uses continuous mode on module. Radio module will continuously receive packets and each time execute user callback. It's in case you need to exchange data larger than 255 bytes(FIFO buffer). You have to manually stop receiving by void LoRa_stop_receive(LoRa_ctl *modem)

### 15.01.2019 update

Updated library to handle radio module from user Rx\Tx callbacks. Backward compatibility retained.
Now you can start sending data from Rx callback, or vice versa, or any other operation with radio module.
New examples "ping.c" and "pong.c" to explain new features. Make, then execute on different devices first "pong" then "ping"

Supported by [ITT ltd](http://zaoitt.ru/)