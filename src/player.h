
#pragma once

#include <stdint.h>
#include <string>

namespace player
{
  enum { MAX_UNITS = 128 };

  struct Unit
  {
    float pos[3];
    float vel[3];
  };

  struct Player
  {
    Player();

    float startX, startZ;
    float cameraX, cameraZ, cameraDir;
    float cameraMoveForward, cameraMoveSideways;

    float spawnRate;
    float timeToNextSpawn;

    std::string name;

    Unit units[MAX_UNITS];
    uint32_t unitCount;
  };

  void init();
  void shutdown();

  Player & player();

  void setName(std::string const& name);

  void setCamera();
  void render();
  void tick(double dt);
}