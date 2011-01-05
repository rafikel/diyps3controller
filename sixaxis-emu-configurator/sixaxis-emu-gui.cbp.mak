#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool                               #
#------------------------------------------------------------------------------#

WSDIR = `pwd`


CC = gcc
CPP = g++
F77 = f77
F9X = gfortran
LD = g++
AR = ar
RANLIB = ranlib
WINDRES = windres

INC = -Iinclude -I/usr/include/libxml2
CFLAGS = -Wall `wx-config --cflags` -Winvalid-pch -include wx_pch.h -DWX_PRECOMP
RESINC = 
RCFLAGS = 
LIBDIR =
LIB = -lSDL -lxml2
LDFLAGS = `wx-config --libs`

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/sixaxis-emu-gui

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = -s $(LDFLAGS)
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/sixaxis-emu-gui

OBJ_DEBUG = $(OBJDIR_DEBUG)/src/event_catcher.o $(OBJDIR_DEBUG)/src/XmlWritter.o $(OBJDIR_DEBUG)/src/XmlReader.o $(OBJDIR_DEBUG)/src/Trigger.o $(OBJDIR_DEBUG)/src/Event.o $(OBJDIR_DEBUG)/src/Device.o $(OBJDIR_DEBUG)/src/Controller.o $(OBJDIR_DEBUG)/src/ConfigurationFile.o $(OBJDIR_DEBUG)/src/Configuration.o $(OBJDIR_DEBUG)/src/ButtonMapper.o $(OBJDIR_DEBUG)/src/AxisMapper.o $(OBJDIR_DEBUG)/sixaxis_emu_guiMain.o $(OBJDIR_DEBUG)/sixaxis_emu_guiApp.o
OBJ_RELEASE = $(OBJDIR_RELEASE)/src/event_catcher.o $(OBJDIR_RELEASE)/src/XmlWritter.o $(OBJDIR_RELEASE)/src/XmlReader.o $(OBJDIR_RELEASE)/src/Trigger.o $(OBJDIR_RELEASE)/src/Event.o $(OBJDIR_RELEASE)/src/Device.o $(OBJDIR_RELEASE)/src/Controller.o $(OBJDIR_RELEASE)/src/ConfigurationFile.o $(OBJDIR_RELEASE)/src/Configuration.o $(OBJDIR_RELEASE)/src/ButtonMapper.o $(OBJDIR_RELEASE)/src/AxisMapper.o $(OBJDIR_RELEASE)/sixaxis_emu_guiMain.o $(OBJDIR_RELEASE)/sixaxis_emu_guiApp.o

all: debug release

clean: clean_debug clean_release

debug: $(OUT_DEBUG)

$(OUT_DEBUG): $(OBJ_DEBUG) $(DEP_DEBUG)
	test -d bin/Debug || mkdir -p bin/Debug
	$(LD) $(LDFLAGS_DEBUG) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/src/event_catcher.o: src/event_catcher.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/event_catcher.o src/event_catcher.cpp

$(OBJDIR_DEBUG)/src/XmlWritter.o: src/XmlWritter.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/XmlWritter.o src/XmlWritter.cpp

$(OBJDIR_DEBUG)/src/XmlReader.o: src/XmlReader.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/XmlReader.o src/XmlReader.cpp

$(OBJDIR_DEBUG)/src/Trigger.o: src/Trigger.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/Trigger.o src/Trigger.cpp

$(OBJDIR_DEBUG)/src/Event.o: src/Event.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/Event.o src/Event.cpp

$(OBJDIR_DEBUG)/src/Device.o: src/Device.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/Device.o src/Device.cpp

$(OBJDIR_DEBUG)/src/Controller.o: src/Controller.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/Controller.o src/Controller.cpp

$(OBJDIR_DEBUG)/src/ConfigurationFile.o: src/ConfigurationFile.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/ConfigurationFile.o src/ConfigurationFile.cpp

$(OBJDIR_DEBUG)/src/Configuration.o: src/Configuration.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/Configuration.o src/Configuration.cpp

$(OBJDIR_DEBUG)/src/ButtonMapper.o: src/ButtonMapper.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/ButtonMapper.o src/ButtonMapper.cpp

$(OBJDIR_DEBUG)/src/AxisMapper.o: src/AxisMapper.cpp
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/src/AxisMapper.o src/AxisMapper.cpp

$(OBJDIR_DEBUG)/sixaxis_emu_guiMain.o: sixaxis_emu_guiMain.cpp
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/sixaxis_emu_guiMain.o sixaxis_emu_guiMain.cpp

$(OBJDIR_DEBUG)/sixaxis_emu_guiApp.o: sixaxis_emu_guiApp.cpp
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)
	$(CPP) $(CFLAGS_DEBUG) $(INC_DEBUG) -c -o $(OBJDIR_DEBUG)/sixaxis_emu_guiApp.o sixaxis_emu_guiApp.cpp


clean_debug:
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)

release: $(OUT_RELEASE)

$(OUT_RELEASE): $(OBJ_RELEASE) $(DEP_RELEASE)
	test -d bin/Release || mkdir -p bin/Release
	$(LD) $(LDFLAGS_RELEASE) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/src/event_catcher.o: src/event_catcher.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/event_catcher.o src/event_catcher.cpp

$(OBJDIR_RELEASE)/src/XmlWritter.o: src/XmlWritter.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/XmlWritter.o src/XmlWritter.cpp

$(OBJDIR_RELEASE)/src/XmlReader.o: src/XmlReader.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/XmlReader.o src/XmlReader.cpp

$(OBJDIR_RELEASE)/src/Trigger.o: src/Trigger.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/Trigger.o src/Trigger.cpp

$(OBJDIR_RELEASE)/src/Event.o: src/Event.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/Event.o src/Event.cpp

$(OBJDIR_RELEASE)/src/Device.o: src/Device.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/Device.o src/Device.cpp

$(OBJDIR_RELEASE)/src/Controller.o: src/Controller.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/Controller.o src/Controller.cpp

$(OBJDIR_RELEASE)/src/ConfigurationFile.o: src/ConfigurationFile.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/ConfigurationFile.o src/ConfigurationFile.cpp

$(OBJDIR_RELEASE)/src/Configuration.o: src/Configuration.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/Configuration.o src/Configuration.cpp

$(OBJDIR_RELEASE)/src/ButtonMapper.o: src/ButtonMapper.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/ButtonMapper.o src/ButtonMapper.cpp

$(OBJDIR_RELEASE)/src/AxisMapper.o: src/AxisMapper.cpp
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/src/AxisMapper.o src/AxisMapper.cpp

$(OBJDIR_RELEASE)/sixaxis_emu_guiMain.o: sixaxis_emu_guiMain.cpp
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/sixaxis_emu_guiMain.o sixaxis_emu_guiMain.cpp

$(OBJDIR_RELEASE)/sixaxis_emu_guiApp.o: sixaxis_emu_guiApp.cpp
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)
	$(CPP) $(CFLAGS_RELEASE) $(INC_RELEASE) -c -o $(OBJDIR_RELEASE)/sixaxis_emu_guiApp.o sixaxis_emu_guiApp.cpp


clean_release:
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)

.PHONY: clean clean_debug clean_release
