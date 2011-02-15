prefix=$(DESTDIR)/usr
bindir=$(prefix)/bin
confdir=$(DESTDIR)/etc/emuclient
BINS=emu emuclient sixaddr bdaddr
CC=gcc
CFLAGS=-Wall -g

all: $(BINS)

clean:
	rm -f $(BINS) *~ *.o

emu: emu.o sixaxis.o dump.o l2cap_con.o bt_utils.o
	$(CC) $(CFLAGS) -lbluetooth -o $@ $^

emuclient: emuclient.o sixaxis.o dump.o macros.o config.o conversion.o
	$(CC) $(CFLAGS) -lSDL -lxml2 -o $@ $^

sixaddr: utils/sixaddr.c
	$(CC) $(CFLAGS) -lusb -o $@ $^

bdaddr: utils/bdaddr.c utils/oui.c
	$(CC) $(CFLAGS) -lbluetooth -o $@ $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) -I/usr/include/libxml2
	
install: all
	mkdir -p $(prefix)
	mkdir -p $(bindir)
	for i in $(BINS); do cp $$i $(bindir)/; done
	mkdir -p $(confdir)/config
	mkdir -p $(confdir)/macros
	cp config/* $(confdir)/config
	cp macros/* $(confdir)/macros

uninstall:
	-for i in $(BINS); do rm $(bindir)/$$i; done
	-rmdir $(bindir)
	-rmdir $(prefix)
	-rm -rf $(confdir)

really-clean: clean uninstall
