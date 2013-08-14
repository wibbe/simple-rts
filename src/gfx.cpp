
#include "config.h"
#include "gfx.h"
#include "fpumath.h"

#include "stb_image.cpp"

#define STBTT_malloc(x,u)  malloc(x)
#define STBTT_free(x,u)    free(x)

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.cpp"

#include <map>
#include <set>
#include <string>
#include <cassert>
#include <stdio.h>

namespace gfx
{

  enum ShaderEffect
  {
    VERTEX_COLOR = 1 << 1,
    TEXTURE      = 1 << 2
  };

  const char * vertexShaderCode =
    "attribute vec3 inPosition;\n"
    "#ifdef USE_VERTEX_COLOR\n"
    "  attribute vec4 inVertexColor;\n"
    "  varying vec4 vertexColor;\n"
    "#endif\n"
    "#ifdef USE_TEXTURE\n"
    "  attribute vec2 inTexCoord;\n"
    "  varying vec2 texCoord;\n"
    "#endif\n"
    "#ifdef USE_LIGHTING\n"
    "  attribute vec3 inNormal;\n"
    "#endif\n"
    "uniform mat4 viewProjectionMatrix;\n"
    "uniform mat4 modelMatrix;\n"
    "uniform vec4 texOffset;\n"
    "\n"
    "void main()\n"
    "{\n"
    "  gl_Position = viewProjectionMatrix * modelMatrix * vec4(inPosition, 1.0);\n"
    "  #ifdef USE_VERTEX_COLOR\n"
    "    vertexColor = inVertexColor;\n"
    "  #endif\n"
    "  #ifdef USE_TEXTURE\n"
    "    texCoord = vec2(texOffset.x + inTexCoord.x * texOffset.z,\n"
    "                    texOffset.y + inTexCoord.y * texOffset.w);\n"
    "  #endif\n"
    "}\n";

  static const char * fragmentShaderCode =
    "#ifdef USE_VERTEX_COLOR\n"
    "  varying vec4 vertexColor;\n"
    "#endif\n"
    "#ifdef USE_TEXTURE\n"
    "  varying vec2 texCoord;\n"
    "  uniform sampler2D texture;\n"
    "#endif\n"
    "\n"
    "void main()\n"
    "{\n"
    "  vec4 finalColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "  #ifdef USE_VERTEX_COLOR\n"
    "    finalColor *= vertexColor;\n"
    "  #endif\n"
    "  #ifdef USE_TEXTURE\n"
    "    vec4 textureColor = texture2D(texture, texCoord);\n"
    "    textureColor.rgb *= textureColor.rgb;\n"
    "    finalColor *= textureColor;\n"
    "  #endif\n"
    "  gl_FragColor = vec4(sqrt(finalColor.rgb), finalColor.a);\n"
    "}\n";

  #define CHECK_GL_ERROR() \
  { \
    GLenum errorCode = glGetError(); \
    if (errorCode != GL_NO_ERROR) \
    { \
      fprintf(stderr, "OpenGL Error %s(%d) %d = %s\n", __FILE__, __LINE__, errorCode, glGetString(errorCode)); \
      exit(1); \
    } \
  }

  struct Effect
  {
    uint32_t features;

    GLuint program;
    GLuint positionAttribute;
    GLuint normalAttribute;
    GLuint colorAttribute;
    GLuint texCoordAttribute;
    GLuint viewProjectionUniform;
    GLuint modelUniform;
    GLuint textureUniform;
    GLuint texOffsetUniform;
  };

  struct Texture
  {
    uint32_t name;
    uint32_t width;
    uint32_t height;
  };

  struct Font
  {
  };

  struct VertexBuffer
  {
    GLuint id;
    bool dynamic;
    VertexDecl decl;
  };

  struct IndexBuffer
  {
    GLuint id;
    bool dynamic;
  };

  struct Impl
  {
    std::map<uint32_t, Effect *> effects;
    std::map<std::string, Texture *> textures;
    std::map<std::string, Font *> fonts;

    uint32_t width;
    uint32_t height;

    float fovy;
    float near;
    float far;

    Effect * currentEffect;
    GLuint currentTexture;

    bool debugging;

    GLfloat projMatrix2D[16];
    GLfloat projMatrix3D[16];
    GLfloat viewMatrix3D[16];
  };

  static Impl * _impl = NULL;

  static GLuint compileShader(const char * code, GLenum type)
  {
    GLuint shader = glCreateShader(type);
    int len = strlen(code);

    glShaderSource(shader, 1, &code, &len);
    glCompileShader(shader);

    GLint compileSuccess;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE)
    {
      GLchar message[256];
      glGetShaderInfoLog(shader, sizeof(message), 0, &message[0]);
      fprintf(stderr, "Shader (%s) error: %s\n", (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment"), message);
      exit(1);
    }

    CHECK_GL_ERROR();
    return shader;
  }

  static Effect * getEffect(uint32_t feature, Impl * d)
  {
    std::map<uint32_t, Effect *>::iterator result = d->effects.find(feature);
    if (result != d->effects.end())
      return result->second;

    std::string header;
    if (feature & Feature::Texture)
      header += "#define USE_TEXTURE\n";
    if (feature & Feature::VertexColor)
      header += "#define USE_VERTEX_COLOR\n";
    if (feature & Feature::Lighting)
      header += "#define USE_LIGHTING\n";

    std::string vertexCode = header + std::string(vertexShaderCode);
    std::string fragmentCode = header + std::string(fragmentShaderCode);

    GLuint vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    CHECK_GL_ERROR();

    GLint linkSuccess;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE)
    {
      GLchar message[256];
      glGetProgramInfoLog(program, sizeof(message), 0, &message[0]);
      fprintf(stderr, "%s\n", message);
      exit(1);
    }

    Effect * effect = new Effect;
    effect->features = feature;
    effect->program = program;

    glUseProgram(program);

    effect->positionAttribute = glGetAttribLocation(program, "inPosition");

    if (feature & Feature::Lighting)
      effect->normalAttribute = glGetAttribLocation(program, "inNormal");

    if (feature & Feature::VertexColor)
      effect->colorAttribute = glGetAttribLocation(program, "inVertexColor");

    if (feature & Feature::Texture)
    {
      effect->texCoordAttribute = glGetAttribLocation(program, "inTexCoord");
      effect->textureUniform = glGetUniformLocation(program, "texture");
      effect->texOffsetUniform = glGetUniformLocation(program, "texOffset");
      glUniform1i(effect->textureUniform, 0);
    }

    effect->viewProjectionUniform = glGetUniformLocation(program, "viewProjectionMatrix");
    effect->modelUniform = glGetUniformLocation(program, "modelMatrix");

    CHECK_GL_ERROR();

    d->effects.insert(std::make_pair(feature, effect));
    return effect;
  }

  void init(int width, int height)
  {
    _impl = new Impl();
    _impl->currentEffect = 0;
    _impl->currentTexture = 0;
    _impl->debugging = false;

    glewInit();

    glDisable(GL_CULL_FACE);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    resize(width, height);

    CHECK_GL_ERROR();
  }

  void shutdown()
  {
    for (std::map<uint32_t, Effect *>::iterator it = _impl->effects.begin(); it != _impl->effects.end(); ++it)
      delete it->second;

    for (std::map<std::string, Texture *>::iterator it = _impl->textures.begin(); it != _impl->textures.end(); ++it)
      delete it->second;

    for (std::map<std::string, Font *>::iterator it = _impl->fonts.begin(); it != _impl->fonts.end(); ++it)
      delete it->second;

    delete _impl;
    _impl = NULL;
  }

  // -- Memory --

  static std::set<const Memory *> _allocatedMemory;
  static std::set<const Memory *> _refMemory;

  static void dispose(const Memory * mem)
  {
    if (_allocatedMemory.count(mem) > 0)
      delete[] mem->data;

    _allocatedMemory.erase(mem);
    _refMemory.erase(mem);

    delete mem;
  }

  const Memory * alloc(uint32_t size)
  {
    Memory * mem = new Memory;
    _allocatedMemory.insert(mem);
    mem->data = new uint8_t[size];
    mem->size = size;
    return mem;
  }

  const Memory * makeRef(const void * data, uint32_t size)
  {
    Memory * mem = new Memory;
    _refMemory.insert(mem);
    mem->data = static_cast<uint8_t *>(const_cast<void *>(data));
    mem->size = size;
    return mem;
  }

  // -- Resource loading --

  Texture * loadTexture(const char * filename)
  {
    std::map<std::string, Texture *>::iterator result = _impl->textures.find(filename);
    if (result != _impl->textures.end())
      return result->second;

    CHECK_GL_ERROR();

    unsigned char * image = 0;
    int width, height, channels;

    FILE * file = fopen(filename, "rb");
    if (!file)
    {
      fprintf(stderr, "Could not open file '%s'", filename);
      return 0;
    }

    image = stbi_load_from_file(file, &width, &height, &channels, 4);
    fclose(file);

    if (!image)
    {
      fprintf(stderr, "Failed to load texture '%s'", filename);
      return 0;
    }

    Texture * tex = new Texture();
    tex->width = width;
    tex->height = height;

    glGenTextures(1, &tex->name);
    glBindTexture(GL_TEXTURE_2D, tex->name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(image);
    CHECK_GL_ERROR();
    _impl->textures.insert(std::make_pair(filename, tex));

    return tex;
  }

#if 0
  Font * RendererGL32::loadFont(std::string const& filename, float fontSize, int textureWidth, int textureHeight)
  {
    Font * font = 0;

    unsigned char * data;
    int length;
    if (fileContent(resourcePath(filename), &data, length))
    {
      font = new Font();
      font->size = fontSize;
      unsigned char * alpha = new unsigned char[textureWidth * textureHeight];

      stbtt_bakedchar charDef[96];
      stbtt_BakeFontBitmap(data, 0, fontSize, alpha, textureWidth, textureHeight, 32, 96, charDef);
      delete[] data;

      // Construct an luminance-alpha texture
      unsigned char * image = new unsigned char[textureWidth * textureHeight * 4];
      for (int i = 0, len = textureWidth * textureHeight; i < len; ++i)
      {
        image[i * 4 + 0] = 255;
        image[i * 4 + 1] = 255;
        image[i * 4 + 2] = 255;
        image[i * 4 + 3] = alpha[i];
      }

      // Create the texture
      TextureGL * tex = new TextureGL();
      tex->width = textureWidth;
      tex->height = textureHeight;
      font->texture = tex;

      glGenTextures(1, &tex->name);
      glBindTexture(GL_TEXTURE_2D, tex->name);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      delete[] image;

      // Copy glyph info
      for (int i = 0; i < 96; ++i)
      {
        float x = 0.0f, y = 0.0f;
        Font::Glyph & glyph = font->glyphs[i];
        stbtt_aligned_quad quad;

        stbtt_GetBakedQuad(charDef, textureWidth, textureHeight, i, &x, &y, &quad, 1);

        float offsetX = quad.x0 < 0.0f ? -quad.x0 : 0.0f;
        glyph.topLeft = Vector2(quad.x0 + offsetX, quad.y0);
        glyph.bottomRight = Vector2(quad.x1 + offsetX, quad.y1);
        glyph.uvTopLeft = Vector2(quad.s0, quad.t0);
        glyph.uvBottomRight = Vector2(quad.s1, quad.t1);
        glyph.advance = x;
      }

      delete[] alpha;
    }
    else
      console::error("Could not find font '%s'", filename.c_str());

    return font;
  }
#endif

  // -- Buffer routines --

  VertexBuffer * createVertexBuffer(const Memory * mem, VertexDecl const& decl)
  {
    VertexBuffer * buffer = new VertexBuffer();
    buffer->dynamic = false;
    buffer->decl = decl;

    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ARRAY_BUFFER, mem->size, mem->data, GL_STATIC_DRAW);

    dispose(mem);
    return buffer;
  }

  IndexBuffer * createIndexBuffer(const Memory * mem)
  {
    IndexBuffer * buffer = new IndexBuffer();
    buffer->dynamic = false;

    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem->size, mem->data, GL_STATIC_DRAW);

    dispose(mem);
    return buffer;
  }

  VertexBuffer * createDynamicVertexBuffer(const Memory * mem, VertexDecl const& decl)
  {
    VertexBuffer * buffer = new VertexBuffer();
    buffer->dynamic = true;
    buffer->decl = decl;

    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ARRAY_BUFFER, mem->size, mem->data, GL_DYNAMIC_DRAW);

    dispose(mem);
    return buffer;
  }

  IndexBuffer * createDynamicIndexBuffer(const Memory * mem)
  {
    IndexBuffer * buffer = new IndexBuffer();
    buffer->dynamic = true;

    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem->size, mem->data, GL_DYNAMIC_DRAW);

    dispose(mem);
    return buffer;
  }

  void updateVertexBuffer(VertexBuffer * buffer, const Memory * mem)
  {
    assert(buffer->dynamic);
    glBufferData(GL_ARRAY_BUFFER, mem->size, mem->data, GL_DYNAMIC_DRAW);
    dispose(mem);
  }

  void updateIndexBuffer(IndexBuffer * buffer, const Memory * mem)
  {
    assert(buffer->dynamic);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mem->size, mem->data, GL_DYNAMIC_DRAW);
    dispose(mem);
  }

  void destroyVertexBuffer(VertexBuffer * buffer)
  {
    glDeleteBuffers(1, &buffer->id);
    delete buffer;
  }

  void destroyIndexBuffer(IndexBuffer * buffer)
  {
    glDeleteBuffers(1, &buffer->id);
    delete buffer;
  }

  void setVertexBuffer(VertexBuffer * buffer)
  {
    assert(_impl->currentEffect);

    VertexDecl & decl = buffer->decl;
    Effect * effect = _impl->currentEffect;

    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);

    // Position
    if (decl._position.size)
    {
      glEnableVertexAttribArray(effect->positionAttribute);
      glVertexAttribPointer(effect->positionAttribute, decl._position.size, decl._position.type, decl._position.normalize, decl._stride, (void *)decl._position.offset);
      CHECK_GL_ERROR();
    }

    // Normal
    if (effect->features & Feature::Lighting && decl._normal.size)
    {
      glEnableVertexAttribArray(_impl->currentEffect->normalAttribute);
      glVertexAttribPointer(effect->normalAttribute, decl._normal.size, decl._normal.type, decl._normal.normalize, decl._stride, (void *)decl._normal.offset);
      CHECK_GL_ERROR();
    }

    // Color
    if (effect->features & Feature::VertexColor && decl._color.size)
    {
      glEnableVertexAttribArray(_impl->currentEffect->colorAttribute);
      glVertexAttribPointer(effect->colorAttribute, decl._color.size, decl._color.type, decl._color.normalize, decl._stride, (void *)decl._color.offset);
      CHECK_GL_ERROR();
    }

    // TexCoord
    if (effect->features & Feature::Texture && decl._texCoord.size)
    {
      glEnableVertexAttribArray(_impl->currentEffect->texCoordAttribute);
      glVertexAttribPointer(effect->texCoordAttribute, decl._texCoord.size, decl._texCoord.type, decl._texCoord.normalize, decl._stride, (void *)decl._texCoord.offset);
      CHECK_GL_ERROR();
    }
  }

  void setIndexBuffer(IndexBuffer * buffer)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
    CHECK_GL_ERROR();
  }

  // -- Drawing --

  void begin(uint32_t features)
  {
    assert(_impl->currentEffect == 0);
    _impl->currentEffect = getEffect(features, _impl);
    assert(_impl->currentEffect);

    glUseProgram(_impl->currentEffect->program);

    float viewProjMatrix[16];

    if (features & Feature::Proj2D)
    {
      memcpy(viewProjMatrix, _impl->projMatrix2D, sizeof(float) * 16);
    }
    else
    {
      math::mtxMul(viewProjMatrix, _impl->viewMatrix3D, _impl->projMatrix3D);
    }

    glUniformMatrix4fv(_impl->currentEffect->viewProjectionUniform, 1, GL_FALSE, viewProjMatrix);

    if (features & Feature::Texture)
      glActiveTexture(GL_TEXTURE0);

    CHECK_GL_ERROR();
  }

  void end()
  {
    assert(_impl->currentEffect);

    glUseProgram(0);
    CHECK_GL_ERROR();
    _impl->currentEffect = 0;
  }

  void clear(float r, float g, float b)
  {
    glClearColor(r, g, b, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void draw(uint32_t count)
  {
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
    CHECK_GL_ERROR();
  }

  // -- Transformations --

  static void updateProjectionMatrix()
  {
    const float aspect = (float)_impl->width / (float)_impl->height;

    math::mtxOrtho(_impl->projMatrix2D, 0, _impl->width, _impl->height, 0, -1, 1);
    math::mtxProj(_impl->projMatrix3D, _impl->fovy, aspect, _impl->near, _impl->far);
  }

  void setProjection(float fovy, float near, float far)
  {
    _impl->fovy = fovy;
    _impl->near = near;
    _impl->far = far;

    updateProjectionMatrix();
  }

  void setCamera(float eyeX, float eyeY, float eyeZ, float atX, float atY, float atZ)
  {
    const float eye[3] = { eyeX, eyeY, eyeZ };
    const float at[3] = { atX, atY, atZ };
    math::mtxLookAt(_impl->viewMatrix3D, eye, at);
  }

  void setTransform(const float * transform)
  {
    assert(_impl->currentEffect);
    glUniformMatrix4fv(_impl->currentEffect->modelUniform, 1, GL_FALSE, transform);
  }

  void setTransform(float x, float y, float z, float yaw, float pitch, float roll)
  {
    float rotate[16];
    float trans[16];
    float final[16];

    math::mtxRotateXYZ(rotate, pitch, yaw, roll);
    math::mtxTranslate(trans, x, y, z);
    math::mtxMul(final, rotate, trans);
    setTransform(final);
  }

  void resize(uint32_t width, uint32_t height)
  {
    _impl->width = width;
    _impl->height = height;

    glViewport(0, 0, width, height);
    updateProjectionMatrix();
  }

  // -- VertexDecl --

  static uint32_t getTypeSize(uint32_t type)
  {
    switch (type)
    {
      case GL_FLOAT:
        return sizeof(float);

      case GL_UNSIGNED_BYTE:
        return sizeof(uint8_t);

      case GL_BYTE:
        return sizeof(int8_t);
    }

    return 0;
  }

  VertexDecl::VertexDecl()
    : _stride(0)
  {
    memset(&_position, 0, sizeof(VertexDecl::Element));
    memset(&_normal, 0, sizeof(VertexDecl::Element));
    memset(&_color, 0, sizeof(VertexDecl::Element));
    memset(&_texCoord, 0, sizeof(VertexDecl::Element));
  }

  VertexDecl & VertexDecl::position(uint32_t size, uint32_t type, bool normalize)
  {
    _position.size = size;
    _position.type = type;
    _position.offset = _stride;
    _position.normalize = normalize;
    _stride += getTypeSize(type) * size;
    return *this;
  }

  VertexDecl & VertexDecl::normal(uint32_t size, uint32_t type, bool normalize)
  {
    _normal.size = size;
    _normal.type = type;
    _normal.offset = _stride;
    _normal.normalize = normalize;
    _stride += getTypeSize(type) * size;
    return *this;
  }

  VertexDecl & VertexDecl::color(uint32_t size, uint32_t type, bool normalize)
  {
    _color.size = size;
    _color.type = type;
    _color.offset = _stride;
    _color.normalize = normalize;
    _stride += getTypeSize(type) * size;
    return *this;
  }

  VertexDecl & VertexDecl::texCoord(uint32_t size, uint32_t type, bool normalize)
  {
    _texCoord.size = size;
    _texCoord.type = type;
    _texCoord.offset = _stride;
    _texCoord.normalize = normalize;
    _stride += getTypeSize(type) * size;
    return *this;
  }


}