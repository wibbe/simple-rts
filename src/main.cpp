
#define INCLUDE_MAGIC_MAIN

#include "config.h"
#include "gfx.h"
#include "world.h"

#include <stdio.h>


static SDL_Window * _window = 0;
static SDL_GLContext _context;

struct PosColorVertex
{
  float x;
  float y;
  float z;
  uint32_t rgba;
};

static gfx::VertexDecl PosColorDecl;

static PosColorVertex cubeVertices[8] = {
  {-1.0f, 1.0f, 1.0f, 0xff000000 },
{ 1.0f, 1.0f, 1.0f, 0xff0000ff },
{-1.0f, -1.0f, 1.0f, 0xff00ff00 },
{ 1.0f, -1.0f, 1.0f, 0xff00ffff },
{-1.0f, 1.0f, -1.0f, 0xffff0000 },
{ 1.0f, 1.0f, -1.0f, 0xffff00ff },
{-1.0f, -1.0f, -1.0f, 0xffffff00 },
{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeIndices[36] = {
  0, 1, 2, // 0
  1, 3, 2,
  4, 6, 5, // 2
  5, 6, 7,
  0, 2, 4, // 4
  4, 2, 6,
  1, 5, 3, // 6
  5, 7, 3,
  0, 4, 1, // 8
  4, 5, 1,
  2, 3, 6, // 10
  6, 3, 7,
};

gfx::VertexBuffer * cubeVB = NULL;
gfx::IndexBuffer * cubeIB = NULL;

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
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
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

  PosColorDecl.position(3, GL_FLOAT)
              .color(4, GL_UNSIGNED_BYTE, true);

  const gfx::Memory * mem = gfx::makeRef(cubeVertices, sizeof(cubeVertices));
  cubeVB = gfx::createVertexBuffer(mem, PosColorDecl);

  mem = gfx::makeRef(cubeIndices, sizeof(cubeIndices));
  cubeIB = gfx::createIndexBuffer(mem);

  gfx::setProjection(50, 1.0, 1000.0);
  gfx::setCamera(0, 0, -15, 0, 0, 0);

  world::create_empty(10, 10);

  mainLoop();

  gfx::destroyVertexBuffer(cubeVB);
  gfx::destroyIndexBuffer(cubeIB);

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

  float yaw = 0.0f;

  bool running = true;
  while (running)
  {
    uint64_t now = SDL_GetPerformanceCounter();
    double dt = (now - oldTimeStamp) / (double)SDL_GetPerformanceFrequency();
    oldTimeStamp = now;

    if (dt > 0.1f)
      dt = 1.0f / 60.0f;

    yaw += dt * M_PI * 0.5;

    SDL_GL_SwapWindow(_window);

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

    gfx::clear(0.1, 0.3, 0.4);

    world::render();

    gfx::begin(gfx::Feature::VertexColor | gfx::Feature::Proj3D);

    gfx::setVertexBuffer(cubeVB);
    gfx::setIndexBuffer(cubeIB);
    gfx::setTransform(0, 0, 0, yaw, yaw, 0);

    gfx::draw(36);

    gfx::end();
  }
}
