
cmake_minimum_required(VERSION 2.6)
project(CUBE_RTS)

# Find SDL and other libraries
set(LIBS_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/libs/local CACHE STRING "Path to SDL 2.0.0" FORCE)

find_library(SDL_LIBRARY
             NAMES SDL-2.0.0 SDL2
             PATHS ${LIBS_FOLDER}
             PATH_SUFFIXES lib VisualC/SDL/Release
             NO_DEFAULT_PATH REQUIRED)

find_library(SDLMAIN_LIBRARY
             NAMES SDL2main
             PATHS ${LIBS_FOLDER}
             PATH_SUFFIXES lib VisualC/SDLmain/Release
             NO_DEFAULT_PATH REQUIRED)

find_package(OpenGL REQUIRED)

set(EXTRA_LIBS "z")

if (UNIX)
  if (APPLE)
    set(EXTRA_LIBS ${EXTRA_LIBS} "-framework Cocoa")
    set(EXTRA_LIBS ${EXTRA_LIBS} "-framework ForceFeedback")
    set(EXTRA_LIBS ${EXTRA_LIBS} "-framework IOKit")
    #set(CMAKE_OSX_ARCHITECTURES i386)
  else()
    #set(CMAKE_C_FLAGS -m32)
    #set(CMAKE_CXX_FLAGS -m32)
  endif()
endif()

include_directories(${LIBS_FOLDER}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/enet/include)

set(SOURCE
  src/client.cpp
  src/clientextras.cpp
  src/clientgame.cpp
  src/clients2c.cpp
  src/command.cpp
  src/console.cpp
  src/editing.cpp
  src/entities.cpp
  src/main.cpp
  src/menus.cpp
  src/monster.cpp
  src/physics.cpp
  src/rendercubes.cpp
  src/renderextras.cpp
  src/rendergl.cpp
  src/rendermd2.cpp
  src/renderparticles.cpp
  src/rendertext.cpp
  src/rndmap.cpp
  src/savegamedemo.cpp
  src/server.cpp
  src/serverbrowser.cpp
  src/serverms.cpp
  src/serverutil.cpp
  src/sound.cpp
  src/tools.cpp
  src/weapon.cpp
  src/world.cpp
  src/worldio.cpp
  src/worldlight.cpp
  src/worldocull.cpp
  src/worldrender.cpp

  src/enet/callbacks.c
  src/enet/host.c
  src/enet/list.c
  src/enet/memory.c
  src/enet/packet.c
  src/enet/peer.c
  src/enet/protocol.c
)

# Platform specific source
if (UNIX)
  if (APPLE)
    set(SOURCE ${SOURCE} src/mac.mm src/enet/unix.c)
  else()
    set(SOURCE ${SOURCE} src/unix.cpp src/enet/unix.c)
  endif()
else()
  set(SOURCE ${SOURCE} src/win32.cpp src/enet/win32.c)
endif()

# Build and link app
add_executable(CubeRTS
               WIN32 MACOSX_BUNDLE
               ${SOURCE})

target_link_libraries(CubeRTS
                      ${OPENGL_LIBRARIES}
                      ${SDL_LIBRARY}
                      ${SDLMAIN_LIBRARY}
                      ${EXTRA_LIBS})

# Fix search path to SDL
#if(WIN32)
#  set(VS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
#  add_custom_command(TARGET CubeRTS POST_BUILD
#                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                     ${SDL_FOLDER}/VisualC/SDL/Release/SDL2.dll ${VS_OUTPUT_DIR}/SDL2.dll)
#endif()
#
if(APPLE)
  set(BUNDLE_BINARY ${CMAKE_CURRENT_BINARY_DIR}/CubeRTS.app/Contents/Resources)
  add_custom_command(TARGET CubeRTS POST_BUILD
                     COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy-data-to-bundle.sh ${BUNDLE_BINARY} ${CMAKE_CURRENT_SOURCE_DIR})
endif()
