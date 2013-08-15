
#pragma once

#include "gfx.h"

namespace gfxe
{

  struct PosColorVertex
  {
    float x, y, z;
    uint32_t abgr;
  };

  extern gfx::VertexDecl PosColorVertexDecl;

  void init();
  void shutdown();

  void beginCube();
  void drawCube();
  void endCube();

}