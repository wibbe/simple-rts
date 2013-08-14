#pragma once

// Include windows.h propertly on Windows
#if defined(WIN32) || defined(_WINDOWS)
  #define WIN32_LEAN_AND_MEAN
  //#define NOMINMAX
  #include <windows.h>
#endif

// Include SDL
#include <SDL.h>
//#include <SDL_opengl.h>
#include "GL/glew.h"
#ifndef INCLUDE_MAGIC_MAIN
  #undef main
#endif