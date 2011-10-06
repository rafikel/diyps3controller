#!/bin/bash

CPU=4
PREFIX=/mingw
WXMSW=wxMSW-2.8.12
SDL=SDL-1.2.14
SVN_VERSION=1.6.17
ZLIB=1.2.3

#install required tools/librairies
mingw-get install msys-patch msys-wget msys-unzip mingw32-libiconv

#install svn
if ! test -f /usr/local/bin/svn.exe
then
  wget http://sourceforge.net/projects/win32svn/files/$SVN_VERSION/svn-win32-$SVN_VERSION.zip/download
  unzip svn-win32-$SVN_VERSION.zip
  mkdir -p /usr/local/bin
  mv svn-win32-$SVN_VERSION/bin/* /usr/local/bin/
fi

#build the SDL library
if ! test -d $SDL
then
  if ! test -f $SDL.tar.gz
  then
    wget http://www.libsdl.org/release/$SDL.tar.gz
    tar xzvf $SDL.tar.gz
  fi
  svn export http://diyps3controller.googlecode.com/svn/trunk/libsdl/patch.win
  cd $SDL
  patch -p1 < ../patch.win
  ./configure --enable-stdio-redirect=no --prefix=$PREFIX
  make -j $CPU
  make install
  cd ..
fi

#build wxWidgets
if ! test -d $WXMSW
then
  if ! test -f $WXMSW.zip
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

