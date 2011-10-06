#!/bin/bash

CPU=4
PREFIX=/mingw
WXMSW=wxMSW-2.8.12
SDL=SDL-1.2.14
ZLIB=1.2.3

#install required tools/librairies
test -f /bin/patch || mingw-get install msys-patch
test -f /bin/wget || mingw-get install msys-wget
test -f /bin/unsizp || mingw-get install msys-unzip
test -f $PREFIX/bin/libiconv-2.dll || mingw-get install mingw32-libiconv

#build the SDL library
if ! test -f $PREFIX/bin/SDL.dll
then
  if ! test -d $SDL
  then
    wget http://www.libsdl.org/release/$SDL.tar.gz
    tar xzvf $SDL.tar.gz
  fi
  wget http://diyps3controller.googlecode.com/svn/trunk/libsdl/patch.win
  cd $SDL
  patch -p1 < ../patch.win
  ./configure --enable-stdio-redirect=no --prefix=$PREFIX
  make -j $CPU
  make install
  cd ..
fi

#build wxWidgets
if ! test -f libwx_baseu-*
then
  if ! test -d $WXMSW
  then
    wget http://prdownloads.sourceforge.net/wxwindows/$WXMSW.zip
    unzip $WXMSW.zip
  fi
  cd $WXMSW
  mkdir msw-gimx
  cd msw-gimx
  ../configure --disable-shared --enable-unicode --prefix=$PREFIX
  make -j $CPU
  make install
  cd ../..
fi

#Get libxml
if ! test -f $PREFIX/bin/libxml2.dll
then
  if ! test -d libxml
  then
    mkdir libxml
	  cd libxml
    wget http://sourceforge.net/projects/devpaks/files/libxml2/LibXML2%20-%202.6.27/libxml2-2.6.27-1cm.DevPak/download
	tar xjvf libxml2-2.6.27-1cm.DevPak
  fi  
  cp bin/* $PREFIX/bin
  cp -r include/libxml $PREFIX/include
  cp lib/* $PREFIX/lib
  cd ..
fi

#Get zlib
if ! test -f $PREFIX/bin/zlib1.dll
then
  if ! test -d zlib
  then
    mkdir zlib
	cd zlib
    wget http://sourceforge.net/projects/gnuwin32/files/zlib/$ZLIB/zlib-$ZLIB-bin.zip/download
  fi
  unzip zlib-$ZLIB-bin.zip
  cp bin/zlib1.dll $PREFIX/bin
  cd ..
fi

