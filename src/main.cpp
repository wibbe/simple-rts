
#define INCLUDE_MAGIC_MAIN

#include "config.h"
#include "gfx.h"

#include <stdio.h>


static SDL_Window * _window = 0;
static SDL_GLContext _context;

static void criticalError(const char *title, const char * text)
{
#if defined(WIN32) || defined(_WINDOWS)
  MessageBoxA(0, text, title, MB_OK | MB_SETFOREGROUND | MB_ICONSTOP);
#else
  fprintf(stderr, "%s - %s", title, text);
#endif
  exit(1);
}

void mainLoop();

#if defined(WIN32) || defined(_WINDOWS)
int SDL_main(int argc, char * argv[])
#else
int main(int argc, char * argv[])
#endif
{
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
  //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    criticalError("Could not initialize SDL", SDL_GetError());

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

  _window = SDL_CreateWindow("Simple RTS",
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             1280, 720,
                             SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | (false ? SDL_WINDOW_FULLSCREEN : 0));

  if (!_window)
    criticalError("Unable to create SDL window", SDL_GetError());

  _context = SDL_GL_CreateContext(_window);
  SDL_GL_SetSwapInterval(1);

  gfx::init(1280, 720);

  // Make sure we are always running on the same core, otherwise we can get timing issues
  #if defined(WIN32) || defined(_WINDOWS)
    ULONG_PTR affinity_mask;
    ULONG_PTR process_affinity_mask;
    ULONG_PTR system_affinity_mask;

    if (!GetProcessAffinityMask(GetCurrentProcess(), &process_affinity_mask, &system_affinity_mask))
      return 0;

    // run on the first core
    affinity_mask = (ULONG_PTR)1 << 0;
    if (affinity_mask & process_affinity_mask)
      SetThreadAffinityMask(GetCurrentThread(), affinity_mask);
  #endif

  mainLoop();

  gfx::shutdown();

  SDL_GL_DeleteContext(_context);
  SDL_DestroyWindow(_window);
  SDL_Quit();

  return 0;
}

void mainLoop()
{
  SDL_Event event;
  uint64_t oldTimeStamp = SDL_GetPerformanceCounter();

  bool running = true;
  while (running)
  {
    uint64_t now = SDL_GetPerformanceCounter();
    double dt = (now - oldTimeStamp) / (double)SDL_GetPerformanceFrequency();
    oldTimeStamp = now;

    if (dt > 0.1f)
      dt = 1.0f / 60.0f;

    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
          running = false;
          break;

        case SDL_TEXTINPUT:
          {
            //TextInputEvent * textEvent = new TextInputEvent(event.text.text);
          }
          break;

        case SDL_KEYDOWN:
          {
            //KeyEvent * keyEvent = new KeyEvent(sdlToEngine(event.key.keysym.sym), true);
          }
          break;

        case SDL_KEYUP:
          {
            //KeyEvent * keyEvent = new KeyEvent(sdlToEngine(event.key.keysym.sym), false);
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          {
            //MouseEvent * mouseEvent = new MouseEvent((Mouse::Values)event.button.button, true, event.button.x, event.button.y);
          }
          break;

        case SDL_MOUSEBUTTONUP:
          {
            //MouseEvent * mouseEvent = new MouseEvent((Mouse::Values)event.button.button, false, event.button.x, event.button.y);
          }
          break;

        case SDL_MOUSEMOTION:
          {
            //MouseEvent * mouseEvent = new MouseEvent(Mouse::None, false, event.motion.x, event.motion.y);
          }
          break;

      }
    }

    SDL_GL_SwapWindow(_window);
  }
}
