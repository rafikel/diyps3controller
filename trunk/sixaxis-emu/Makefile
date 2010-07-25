CC=gcc
CFLAGS=-Wall
#CFLAGS+=-DJOYSTICK

.PHONY: all
all: emu emuclient

.PHONY: clean
clean:
	rm -f emu emuclient emucurses *~ *.o

emu: emu.o sixaxis.o dump.o l2cap_con.o
	$(CC) -lbluetooth -o $@ $^

emuclient: emuclient.o sixaxis.o dump.o macros.o
	$(CC) -lSDL -o $@ $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
