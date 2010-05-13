CC=gcc
CFLAGS=-Wall

.PHONY: all
all: emu emuclient

.PHONY: clean
clean:
	rm -f emu emuclient emucurses *~ *.o

emu: emu.o sixaxis.o dump.o
	$(CC) -lbluetooth -o $@ $^

emuclient: emuclient.o sixaxis.o dump.o macros.o
	$(CC) -lSDL -o $@ $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
