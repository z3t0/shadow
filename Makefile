CC=avr-gcc
CFLAGS=-I. -mmcu=atmega328p -o sumobot.out

build: sumobot.o
	$(CC) ${CFLAGS} sumobot.c