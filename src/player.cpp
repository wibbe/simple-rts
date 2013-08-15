
#include "player.h"
#include "world.h"
#include "tcl.h"
#include "gfxe.h"

#include <vector>
#include <memory.h>

namespace player
{
  typedef std::vector<Player *> PlayerVector;

  namespace {
    PlayerVector _allPlayers;
    Player * _human;
  }

  // -- Player --

  Player::Player()
    : startX(0),
      startZ(0),
      cameraX(0),
      cameraZ(0),
      spawnRate(10.0),
      timeToNextSpawn(10.0),
      name("noname"),
      unitCount(0)
  {
    memset(units, 0, sizeof(Unit) * MAX_UNITS);
  }

  // -- API --

  void init()
  {
    _human = new Player();
    _allPlayers.push_back(_human);
  }

  void shutdown()
  {
    for (PlayerVector::iterator it = _allPlayers.begin(); it != _allPlayers.end(); ++it)
      delete *it;

    _allPlayers.clear();
  }

  void setName(std::string const& name)
  {
    player().name = name;
  }

  Player & player()
  {
    return *_human;
  };

  static void spawnUnit(Player * player)
  {
    printf("Spawning for player '%s'\n", player->name.c_str());
  }

  void tick(double dt)
  {
    for (PlayerVector::iterator it = _allPlayers.begin(), end = _allPlayers.end(); it != end; ++it)
    {
      Player * player = *it;

      player->timeToNextSpawn -= dt;
      if (player->timeToNextSpawn < 0.0f)
      {
        spawnUnit(player);
        player->timeToNextSpawn = player->spawnRate;
      }
    }
  }

  void setCamera()
  {
    gfx::setCamera(player().cameraX + 10, 15, player().cameraZ + 10, player().cameraX, 0, player().cameraZ);
  }

  void render()
  {
    gfxe::beginCube();

    for (PlayerVector::const_iterator it = _allPlayers.begin(), end = _allPlayers.end(); it != end; ++it)
    {
      Player * player = *it;

      const float startY = world::getHeight(player->startX, player->startZ);

      // Draw spawn point
      gfx::setTintColor(1, 0, 0);
      gfx::setTransform(player->startX, startY + 0.25, player->startZ, 0, 0, 0, 1.5, 1, 1.5);
      gfxe::drawCube(); // Walls

      gfx::setTintColor(0.1, 0.1, 0.1);
      gfx::setTransform(player->startX, startY + 0.75, player->startZ, 45, 0, 0, 1.45, 1.1, 1.1);
      gfxe::drawCube(); // Roof

    }

    gfxe::endCube();
  }

  // -- Tcl Bindings --

  PROC("player::setName", setName)

}