
#pragma once

#include <stdint.h>

namespace gfx
{
  struct Texture;
  struct Font;
  struct VertexBuffer;
  struct IndexBuffer;

  struct Feature
  {
    enum
    {
      Texture        = 1 << 1,
      VertexColor    = 1 << 2,
      Lighting       = 1 << 3,
      Proj2D         = 1 << 10,
      Proj3D         = 1 << 11
    };
  };

  struct VertexDecl
  {
    struct Element
    {
      uint16_t size;
      uint16_t type;
      uint32_t offset;
      bool normalize;
    };

    VertexDecl();

    VertexDecl & position(uint32_t size, uint32_t type, bool normalize = false);
    VertexDecl & normal(uint32_t size, uint32_t type, bool normalize = false);
    VertexDecl & color(uint32_t size, uint32_t type, bool normalize = false);
    VertexDecl & texCoord(uint32_t size, uint32_t type, bool normalize = false);

    Element _position;
    Element _normal;
    Element _color;
    Element _texCoord;
    uint32_t _stride;
  };

  struct Memory
  {
    uint8_t * data;
    uint32_t size;
  };

  void init(int width, int height);
  void shutdown();

  void resize(uint32_t width, uint32_t height);

  Texture * loadTexture(const char * filename);
  //Font * loadFont(const char * filename, float fontSize, int texWidth, int texHeight);

  const Memory * alloc(uint32_t size);
  const Memory * makeRef(const void * data, uint32_t size);

  VertexBuffer * createVertexBuffer(const Memory * mem, VertexDecl const& decl);
  IndexBuffer * createIndexBuffer(const Memory * mem);

  VertexBuffer * createDynamicVertexBuffer(const Memory * mem, VertexDecl const& decl);
  IndexBuffer * createDynamicIndexBuffer(const Memory * mem);

  void destroyVertexBuffer(VertexBuffer * buffer);
  void destroyIndexBuffer(IndexBuffer * buffer);

  void updateVertexBuffer(VertexBuffer * buffer, const Memory * mem);
  void updateIndexBuffer(IndexBuffer * buffer, const Memory * mem);

  void setVertexBuffer(VertexBuffer * buffer);
  void setIndexBuffer(IndexBuffer * buffer);

  void setProjection(float fovy, float near, float far);
  void setCamera(float eyeX, float eyeY, float eyeZ, float atX, float atY, float atZ);

  void setTransform(const float * transform);
  void setTransform(float x, float y, float z, float yaw, float pitch, float roll);

  void begin(uint32_t features);
  void end();

  void clear(float r, float g, float b);
  void draw(uint32_t count);

}