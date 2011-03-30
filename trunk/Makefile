DIRS=sixaxis-emu sixaxis-emu-configurator sixemugui sixstatus
ARCH=`uname -m`

all:
	for i in $(DIRS); do cd $$i; make all; cd ..; done

clean:
	for i in $(DIRS); do cd $$i; make clean; cd ..; done

install: all
	for i in $(DIRS); do cd $$i; make install; cd ..; done
	mkdir -p $(DESTDIR)/usr/lib
	case ${ARCH} in \
	    x86_64) cp libsdl/lib/libSDL.so.amd64 $(DESTDIR)/usr/lib/libSDL-9.2.so.0 ;; \
	    *) cp libsdl/lib/libSDL.so $(DESTDIR)/usr/lib/libSDL-9.2.so.0 ;; \
	esac
	

uninstall:
	-for i in $(DIRS); do cd $$i; make uninstall; cd ..; done
	-rm $(DESTDIR)/usr/lib/libSDL-9.2.so.0
	-rmdir $(DESTDIR)/usr/lib
	-rmdir $(DESTDIR)/usr

really-clean: clean uninstall
