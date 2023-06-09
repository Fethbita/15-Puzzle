CC = clang
CFLAGS = -g -Wall
LFLAGS = -lwiringPi -lwiringPiDev

.PHONY: all clean

puzzle_rpi: puzzle_rpi.c
	$(CC) $(CFLAGS) $(LFLAGS) -o puzzle_rpi puzzle_rpi.c

clean:
	rm -f puzzle_rpi.c
