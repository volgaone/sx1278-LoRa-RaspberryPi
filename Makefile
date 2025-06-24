#all: receive_implicit receive_explicit transmit_implicit transmit_explicit ping pong
all: transmit_explicit

mySPI.o: mySPI.c
	gcc -c mySPI.c -o mySPI.o -lrt -pthread  -lm
myGPIO.o: mygpio.c
	gcc -c mygpio.c -o mygpio.o -lrt -pthread -lm

LoRa.o: LoRa.c
	gcc -c LoRa.c -o LoRa.o -lrt -pthread -lm


# tx_implicit_example.o: tx_implicit_example.c
# 	gcc -c tx_implicit_example.c -o tx_implicit_example.o  -lrt -pthread -lm

# rx_implicit_example.o: rx_implicit_example.c
# 	gcc -c rx_implicit_example.c -o rx_implicit_example.o  -lrt -pthread -lm

tx_explicit_example.o: tx_explicit_example.c
	gcc -c tx_explicit_example.c -o tx_explicit_example.o  -lrt -pthread -lm

# rx_explicit_example.o: rx_explicit_example.c
# 	gcc -c rx_explicit_example.c -o rx_explicit_example.o  -lrt -pthread -lm
	
# ping.o: ping.c
# 	gcc -c ping.c -o ping.o  -lrt -pthread -lm
	
# pong.o: pong.c
# 	gcc -c pong.c -o pong.o  -lrt -pthread -lm

# ping: LoRa.o ping.o
# 	gcc -o ping ping.o LoRa.o  -lrt -pthread -lm

# pong: LoRa.o pong.o
# 	gcc -o pong pong.o LoRa.o  -lrt -pthread -lm
	
transmit_explicit: LoRa.o tx_explicit_example.o mySPI.o  mygpio.o
	gcc -g -o transmit_explicit tx_explicit_example.o LoRa.o mygpio.o mySPI.o  -lrt -pthread -lm -lwiringPi -l:libgpiolib.a

# transmit_implicit: LoRa.o tx_implicit_example.o
# 	gcc -o transmit_implicit tx_implicit_example.o LoRa.o  -lrt -pthread -lm

# receive_explicit: LoRa.o rx_explicit_example.o
# 	gcc -o receive_explicit rx_explicit_example.o LoRa.o  -lrt -pthread -lm

# receive_implicit: LoRa.o rx_implicit_example.o
# 	gcc -o receive_implicit rx_implicit_example.o LoRa.o  -lrt -pthread -lm

clean:
	rm *.o receive_*plicit transmit_*plicit ping pong
