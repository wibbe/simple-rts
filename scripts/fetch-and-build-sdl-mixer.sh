#!/bin/sh

LIBS_DIR=`pwd`/libs
DOWNLOAD_DIR=$LIBS_DIR/download
SDL_DIR=$DOWNLOAD_DIR/SDL_mixer-1.2.12
BUILD_DIR=$SDL_DIR/build
PREFIX_LIB=$LIBS_DIR/local
ARCHIVE="SDL_mixer-1.2.12.tar.gz"
URL="http://www.libsdl.org/projects/SDL_mixer/release/SDL_mixer-1.2.12.tar.gz"

if [ ! -d "$DOWNLOAD_DIR" ] ; then
  echo "Creating libs/ directory"
  mkdir -p $DOWNLOAD_DIR
fi

cd $DOWNLOAD_DIR

# Make sure we make a clean checkout
rm -rf $SDL_DIR

if [ ! -f $ARCHIVE  ] ; then
  echo "Downloading SDL_mixer..."
  wget $URL
fi

echo "Unpacking SDL_mixer..."
tar xzf $ARCHIVE

cd $SDL_DIR

# Fix some problems in the makefile
sed 's/\$(objects)\/playwave.lo \$(SDL_CFLAGS)/$(objects)\/playwave.lo $(SDL_CFLAGS) $(CFLAGS)/g' Makefile.in > tmp && mv tmp Makefile.in
sed 's/\$(objects)\/playmus.lo \$(SDL_CFLAGS)/$(objects)\/playmus.lo $(SDL_CFLAGS) $(CFLAGS)/g' Makefile.in > tmp && mv tmp Makefile.in

mkdir -p $BUILD_DIR
cd $BUILD_DIR

echo "Configure SDL_mixer..."

#if [ `uname` = "Darwin" ] ; then
#  CFLAGS="-arch i386"
#  CXXFLAGS="-arch i386"
#  LDFLAGS="-arch i386"
#else
#  CFLAGS="-m32"
#  CXXFLAGS="-m32"
#  LDFLAGS="-m32"
#fi

#CFLAGS=$CFLAGS CXXFLAGS=$CXXFLAGS LDFLAGS=$LDFLAGS ../configure --prefix="$PREFIX_LIB"  --with-sdl-prefix="$PREFIX_LIB" --disable-assembly --disable-nasm --disable-static --disable-sdltest --disable-music-mod --disable-music-mod-shared --disable-music-mod-modplug --disable-music-midi --disable-music-timidity-midi --disable-music-native-midi --disable-music-fluidsynth-midi --disable-music-ogg --disable-music-flac --disable-music-mp3 --disable-smpegtest
../configure --prefix="$PREFIX_LIB"  --with-sdl-prefix="$PREFIX_LIB" --disable-assembly --disable-nasm --disable-static --disable-sdltest --disable-music-mod --disable-music-mod-shared --disable-music-mod-modplug --disable-music-midi --disable-music-timidity-midi --disable-music-native-midi --disable-music-fluidsynth-midi --disable-music-ogg --disable-music-flac --disable-music-mp3 --disable-smpegtest

echo "Building SDL_mixer..."
make && make install
