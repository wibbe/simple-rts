
#include "gfxe.h"
#include "config.h"

namespace gfxe
{
  namespace {
    gfx::VertexBuffer * _cubeVB;
    gfx::IndexBuffer * _cubeIB;
  }

  gfx::VertexDecl PosColorVertexDecl;

  static PosColorVertex cubeVertices[8] = {
    {-0.5f, 0.5f, 0.5f, 0xffffffff },
    { 0.5f, 0.5f, 0.5f, 0xffffffff },
    {-0.5f, -0.5f, 0.5f, 0xffffffff },
    { 0.5f, -0.5f, 0.5f, 0xffffffff },
    {-0.5f, 0.5f, -0.5f, 0xffffffff },
    { 0.5f, 0.5f, -0.5f, 0xffffffff },
    {-0.5f, -0.5f, -0.5f, 0xffffffff },
    { 0.5f, -0.5f, -0.5f, 0xffffffff },
  };

  static const uint16_t cubeIndices[36] = {
    0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6, 1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
  };


  void init()
  {
    PosColorVertexDecl.position(3, GL_FLOAT)
                      .color(4, GL_UNSIGNED_BYTE, true);

    // Cube
    const gfx::Memory * mem = gfx::makeRef(cubeVertices, sizeof(cubeVertices));
    _cubeVB = gfx::createVertexBuffer(mem, PosColorVertexDecl);

    mem = gfx::makeRef(cubeIndices, sizeof(cubeIndices));
    _cubeIB = gfx::createIndexBuffer(mem);
  }

  void shutdown()
  {
    gfx::destroyVertexBuffer(_cubeVB);
    gfx::destroyIndexBuffer(_cubeIB);
  }

  void beginCube()
  {
    gfx::begin(gfx::Feature::VertexColor | gfx::Feature::TintColor | gfx::Feature::Proj3D);
    gfx::setTintColor(1, 1, 1);
    gfx::setVertexBuffer(_cubeVB);
    gfx::setIndexBuffer(_cubeIB);
  }

  void drawCube()
  {
    gfx::draw(36);
  }

  void endCube()
  {
    gfx::end();
  }

}