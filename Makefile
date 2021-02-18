SDK_HOME = /home/pi/source/github/esp-open-sdk/sdk
CC = $(SDK_HOME)/../xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
CFLAGS = -I. -mlongcalls -I$(SDK_HOME)/include -I$(SDK_HOME)/driver_lib/driver -I$(SDK_HOME)/driver_lib/include/driver
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -ldriver -lmbedtls -lssl -lcrypto -lwps -Wl,--end-group -lgcc
LDFLAGS = -T$(SDK_HOME)/ld/eagle.app.v6.ld -L$(SDK_HOME)/lib
PROGRAM = main

.PHONY:
all: clean $(PROGRAM)

$(PROGRAM)-0x00000.bin: $(PROGRAM)
	esptool.py elf2image $^

$(PROGRAM): $(PROGRAM).o

$(PROGRAM).o: $(PROGRAM).c

flash: clean $(PROGRAM)-0x00000.bin
	esptool.py write_flash --flash_mode qio 0x0 $(PROGRAM)-0x00000.bin 0x10000 $(PROGRAM)-0x10000.bin

clean:
	rm -f $(PROGRAM) $(PROGRAM).o $(PROGRAM)-0x00000.bin $(PROGRAM)-0x10000.bin
