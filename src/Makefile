CC=avr-gcc

default: flash


build:
	$(CC) -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o sumobot.o sumobot.c
	$(CC) -mmcu=atmega328p sumobot.o -o sumobot
	rm sumobot.o

flash: build
	avr-objcopy -O ihex -R .eeprom sumobot sumobot.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/tty.usbmodem1411 -b 115200 -U flash:w:sumobot.hex


clean:
	rm sumobot.out > /dev/null
	rm sumobot.hex > /dev/null
	rm sumobot > /dev/null