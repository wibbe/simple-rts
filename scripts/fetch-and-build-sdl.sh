#!/bin/sh

LIBS_DIR=`pwd`/libs
DOWNLOAD_DIR=$LIBS_DIR/download
SDL_DIR=$DOWNLOAD_DIR/SDL2-2.0.0
BUILD_DIR=$SDL_DIR/build
PREFIX_LIB=$LIBS_DIR/local
ARCHIVE="SDL2-2.0.0.tar.gz"
#URL="http://www.libsdl.org/release/SDL-1.2.15.tar.gz"

URL="http://www.libsdl.org/release/SDL2-2.0.0.tar.gz"

if [ ! -d "$DOWNLOAD_DIR" ] ; then
  echo "Creating libs/ directory"
  mkdir -p $DOWNLOAD_DIR
fi

cd $DOWNLOAD_DIR

# Make sure we make a clean checkout
rm -rf $SDL_DIR

if [ ! -f $ARCHIVE  ] ; then
  echo "Downloading SDL2..."
  wget $URL
fi

echo "Unpacking SDL2..."
tar xzf $ARCHIVE

cd $SDL_DIR

mkdir -p $BUILD_DIR
cd $BUILD_DIR

echo "Configure SDL2..."

cmake -DSDL_SHARED=OFF ..

echo "Building SDL2..."

make

echo "Installing SDL2..."

mkdir -p $PREFIX_LIB/lib

cp $BUILD_DIR/libSDL2.a $PREFIX_LIB/lib
cp $BUILD_DIR/libSDL2main.a $PREFIX_LIB/lib
cp -r $BUILD_DIR/include $PREFIX_LIB
cp $SDL_DIR/include/*.h $PREFIX_LIB/include

