
#include "config.h"
#include "gfx.h"
#include "tcl.h"
#include "world.h"

#include <stdio.h>
#include <memory.h>

#include <vector>

namespace world
{
  namespace {
    uint32_t _width;
    uint32_t _height;

    uint16_t * _cells = NULL;

    gfx::VertexDecl _terrainDecl;
    gfx::VertexBuffer * _terrainVB = NULL;
    gfx::IndexBuffer * _terrainIB = NULL;
  }

  #define ACCESS(x, z) _cells[((z) > _height ? (_height - 1) : (z)) * _width + ((x) > _width ? (_width - 1) : (x))]
  #define TYPE(cell) (cell & 0x0F)

  struct TerrainVertex
  {
    float x, y, z;
    uint32_t color;
  };

  void clear()
  {
    delete[] _cells;
    _cells = NULL;

    if (_terrainVB)
      gfx::destroyVertexBuffer(_terrainVB);
    if (_terrainIB)
      gfx::destroyIndexBuffer(_terrainIB);

    _terrainVB = NULL;
    _terrainIB = NULL;
  }

  static void initGfx()
  {
    static bool initialized = false;

    if (!initialized)
    {
      _terrainDecl.position(3, GL_FLOAT)
                  .color(4, GL_UNSIGNED_BYTE, true);
      initialized = true;
    }

    /*
    std::vector<TerrainVertex> vb;
    std::vector<uint16_t> ib;

    const float startX = _width * -0.5f;
    const float startZ = _height * -0.5f;

    for (uint32_t x = 0; x < _width; ++x)
      for (uint32_t z = 0; z < _height; ++z)
      {
        uint16_t cell = ACCESS(x, z);
        uint8_t type = TYPE(cell);

        TerrainVertex vertex = { startX + x, 0, startZ + z, 0xff00ff00 };
        vb.push_back(vertex);
      }
    */

    const float halfX = _width * 0.5f;
    const float halfZ = _height * 0.5f;
    const TerrainVertex terrainVertices[4] = {
      { -halfX, 0, -halfZ, 0xff0ba500 },
      { -halfX, 0,  halfZ, 0xff0ba500 },
      {  halfX, 0,  halfZ, 0xff0ba500 },
      {  halfX, 0, -halfZ, 0xff0ba500 },
    };

    static const uint16_t terrainIndicies[6] = { 0, 1, 2, 2, 3, 0 };

    const gfx::Memory * mem = gfx::makeRef(terrainVertices, sizeof(terrainVertices));
    _terrainVB = gfx::createDynamicVertexBuffer(mem, _terrainDecl);

    mem = gfx::makeRef(terrainIndicies, sizeof(terrainIndicies));
    _terrainIB = gfx::createIndexBuffer(mem);
  }

  void createEmpty(uint32_t width, uint32_t height)
  {
    clear();

    _width = width;
    _height = height;

    _cells = new uint16_t[_width * _height];
    memset(_cells, 0, sizeof(uint16_t) * width * height);

    initGfx();
  }

  float getHeight(float x, float y)
  {
    return 0.0f;
  }

  void render()
  {
    if (_terrainVB && _terrainIB)
    {
      gfx::begin(gfx::Feature::VertexColor | gfx::Feature::Proj3D);
      gfx::setTransform(0, 0, 0, 0, 0, 0);
      gfx::setVertexBuffer(_terrainVB);
      gfx::setIndexBuffer(_terrainIB);
      gfx::draw(6);
      gfx::end();
    }
  }

  // Tcl Bindings
  PROC("world:clear", clear)
  PROC("world:createEmpty", createEmpty)
}
