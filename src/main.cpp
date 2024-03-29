
#define INCLUDE_MAGIC_MAIN

#include "config.h"
#include "gfx.h"
#include "gfxe.h"
#include "tcl.h"
#include "world.h"
#include "player.h"
#include "input.h"
#include "platform.h"

#include <stdio.h>

static SDL_Window * _window = 0;
static SDL_GLContext _context;
static bool _running = true;

static void quit()
{
  _running = false;
}

static void criticalError(const char * title, const char * text)
{
  platform::messageBox(title, text);
  exit(1);
}

PROC("quit", quit);
PROC("criticalError", criticalError);

void mainLoop();

#if defined(WIN32) || defined(_WINDOWS)
  int SDL_main(int argc, char * argv[])
#else
  int main(int argc, char * argv[])
#endif
{
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
  //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  tcl::init();
  player::init();
  input::init();

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
  gfxe::init();

  tcl::exec("data/default.tcl");

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

  gfx::setProjection(50, 1.0, 1000.0);

  // Create an empty default world
  world::createEmpty(100, 100);

  mainLoop();

  world::clear();

  player::shutdown();
  gfxe::shutdown();
  gfx::shutdown();
  tcl::shutdown();
  input::shutdown();

  SDL_GL_DeleteContext(_context);
  SDL_DestroyWindow(_window);
  SDL_Quit();

  return 0;
}

static std::string toKeyStr(int key)
{
  switch (key)
  {
    case SDLK_ESCAPE:
      return "ESCAPE";

    case SDLK_RETURN:
      return "RETURN";

    case SDLK_LEFT:
      return "LEFT";

    case SDLK_RIGHT:
      return "RIGHT";

    case SDLK_UP:
      return "UP";

    case SDLK_DOWN:
      return "DOWN";

    default:
      {
        char buf[4];
        snprintf(buf, 4, "%c", toupper(key));
        return buf;
      }
  }
}

void mainLoop()
{
  SDL_Event event;
  uint64_t oldTimeStamp = SDL_GetPerformanceCounter();

  while (_running)
  {
    uint64_t now = SDL_GetPerformanceCounter();
    double dt = (now - oldTimeStamp) / (double)SDL_GetPerformanceFrequency();
    oldTimeStamp = now;

    if (dt > 0.1f)
      dt = 1.0f / 60.0f;

    SDL_GL_SwapWindow(_window);

    player::tick(dt);

    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
          _running = false;
          break;

        case SDL_TEXTINPUT:
          {
            //TextInputEvent * textEvent = new TextInputEvent(event.text.text);
          }
          break;

        case SDL_KEYDOWN:
          input::key(toKeyStr(event.key.keysym.sym), true);
          break;

        case SDL_KEYUP:
          input::key(toKeyStr(event.key.keysym.sym), false);
          break;

        case SDL_MOUSEBUTTONDOWN:
          input::setMousePos(event.button.x, event.button.y);
          switch (event.button.button)
          {
            case SDL_BUTTON_LEFT:
              input::key("MOUSE_LEFT", true);
              break;

            case SDL_BUTTON_MIDDLE:
              input::key("MOUSE_MIDDLE", true);
              break;

            case SDL_BUTTON_RIGHT:
              input::key("MOUSE_RIGHT", true);
              break;
          }
          break;

        case SDL_MOUSEBUTTONUP:
          input::setMousePos(event.button.x, event.button.y);
          switch (event.button.button)
          {
            case SDL_BUTTON_LEFT:
              input::key("MOUSE_LEFT", false);
              break;

            case SDL_BUTTON_MIDDLE:
              input::key("MOUSE_MIDDLE", false);
              break;

            case SDL_BUTTON_RIGHT:
              input::key("MOUSE_RIGHT", false);
              break;
          }
          break;

        case SDL_MOUSEMOTION:
          input::setMousePos(event.motion.x, event.motion.y);
          break;

      }
    }

    gfx::clear(0.1, 0.3, 0.4);

    player::setCamera();
    world::render();
    player::render();
  }
}
