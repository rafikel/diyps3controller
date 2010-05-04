CFLAGS=-Wall

.PHONY: all
all: emu emuclient

.PHONY: clean
clean:
	rm -f emu emuclient emucurses *~ *.o

emu: emu.o sixaxis.o dump.o
	$(LINK.c) -lbluetooth -o $@ $^

emuclient.o: sixaxis.h dump.h

emuclient: emuclient.o sixaxis.o dump.o
	$(LINK.c) -lSDL -o $@ $^

emucurses.o: sixaxis.h dump.h

emucurses: emucurses.o sixaxis.o dump.o
	$(LINK.c) -lcurses -o $@ $^

sixaxis.o: sixaxis.c sixaxis.h

dump.o: dump.c sixaxis.h

emu.o: emu.c sixaxis.h
