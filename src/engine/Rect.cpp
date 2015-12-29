/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Rect.hpp"
#include "Programs.hpp"

namespace
{
   /// @note flip of Y coordinates, because loaded data are top-row-first, but
   /// opengl expects bottom-row-first
   const typename glm::vec2 uvData[] = {
      {0.f, 1.f},
      {1.f, 1.f},
      {1.f, 0.f},
      {0.f, 0.f}
   };

   const glm::vec3 posVertexData[] = {
      {-0.5f,-0.5f, 0.f},
      { 0.5f,-0.5f, 0.f},
      { 0.5f, 0.5f, 0.f},
      {-0.5f, 0.5f, 0.f}
   };

   const GLubyte indices[] = {0, 1, 3, 2};

   auto uvBuffer()
   {
      static auto buffer = std::make_shared<TBufferObject<glm::vec2>>(uvData, sizeof(uvData)/sizeof(uvData[0]));
      return buffer;
   }

   auto posBuffer()
   {
      static auto buffer = std::make_shared<TBufferObject<glm::vec3>>(posVertexData, sizeof(posVertexData)/sizeof(posVertexData[0]));
      return buffer;
   }
}

void CTexturedRect::draw(const SContext&) const
{
   auto& p = gRenderer.get<cts("regular-tex")>();
   p.set<cts("aPos")>(posBuffer());
   p.set<cts("aUV")>(uvBuffer());
   p.set<cts("uModel")>(model());
   p.set<cts("uTexture")>(mTexture);
   p.draw(indices, 4, GL_TRIANGLE_STRIP);
}

void CTexturedBillboard::draw(const SContext&) const
{
   auto& p = gRenderer.get<cts("billboard-tex")>();
   p.set<cts("aPos")>(posBuffer());
   p.set<cts("aUV")>(uvBuffer());
   p.set<cts("uPos")>(mPos);
   p.set<cts("uSize")>(mSize);
   p.set<cts("uTexture")>(mTexture);
   p.draw(indices, 4, GL_TRIANGLE_STRIP);
}

void CAnimatedBillboard::draw(const SContext& context) const
{
   auto& p = gRenderer.get<cts("billboard-tex-sprite")>();
   p.set<cts("aPos")>(posBuffer());
   p.set<cts("aUV")>(uvBuffer());
   p.set<cts("uPos")>(mPos);
   p.set<cts("uSize")>(mSize);
   p.set<cts("uAtlasSize")>(mAtlasSize);
   p.set<cts("uAtlasPos")>(mAtlasPos);
   p.set<cts("uTexture")>(mTexture);
   p.draw(indices, 4, GL_TRIANGLE_STRIP);
}

void CHealthBar::draw(const SContext&) const
{
   auto& p = gRenderer.get<cts("billboard-hb")>();
   p.set<cts("aPos")>(posBuffer());
   p.set<cts("uPos")>(mPos);
   p.set<cts("uSize")>(mSize);
   p.set<cts("uValue")>(mValue);

   gl(glDisable, GL_DEPTH_TEST);
   p.draw(indices, 4, GL_TRIANGLE_STRIP);
   gl(glEnable, GL_DEPTH_TEST);
}
