/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Rect.hpp"
#include "Programs.hpp"

namespace
{
   auto& indexBuffer()
   {
      static const GLubyte data[] = {0, 1, 3, 2};
      static auto buffer = make_indexBuffer(data, sizeof(data)/sizeof(data[0]), GL_TRIANGLE_STRIP);
      return buffer;
   }

   auto& uvBuffer()
   {
      /// @note flip of Y coordinates, because loaded data are top-row-first, but
      /// opengl expects bottom-row-first
      static const typename glm::vec2 data[] = {
         {0.f, 1.f},
         {1.f, 1.f},
         {1.f, 0.f},
         {0.f, 0.f}
      };
      static auto buffer = make_buffer<glm::vec2>(data, sizeof(data)/sizeof(data[0]));
      return buffer;
   }

   auto& posBuffer()
   {
      static const glm::vec3 data[] = {
         {-0.5f,-0.5f, 0.f},
         { 0.5f,-0.5f, 0.f},
         { 0.5f, 0.5f, 0.f},
         {-0.5f, 0.5f, 0.f}
      };
      static auto buffer = make_buffer<glm::vec3>(data, sizeof(data)/sizeof(data[0]));
      return buffer;
   }
}

void CTexturedRect::draw(const SContext&) const
{
   static auto vao = gRenderer.make_vao<cts("regular-tex"),
                                        cts("aPos"), cts("aUV"), cts("indices")>(
                                           posBuffer(), uvBuffer(), indexBuffer());
   auto& p = gRenderer.get<cts("regular-tex")>();
   p.set<cts("vao")>(vao);
   p.set<cts("uModel")>(model());
   p.set<cts("uTexture")>(mTexture);
   p.drawElements();
}

void CTexturedBillboard::draw(const SContext&) const
{
   static auto vao = gRenderer.make_vao<cts("billboard-tex"),
                                        cts("aPos"), cts("aUV"), cts("indices")>(
                                           posBuffer(), uvBuffer(), indexBuffer());
   auto& p = gRenderer.get<cts("billboard-tex")>();
   p.set<cts("vao")>(vao);
   p.set<cts("uPos")>(mPos);
   p.set<cts("uSize")>(mSize);
   p.set<cts("uTexture")>(mTexture);
   p.drawElements();
}

void CAnimatedBillboard::draw(const SContext& context) const
{
   static auto vao = gRenderer.make_vao<cts("billboard-tex-sprite"),
                                        cts("aPos"), cts("aUV"), cts("indices")>(
                                           posBuffer(), uvBuffer(), indexBuffer());
   auto& p = gRenderer.get<cts("billboard-tex-sprite")>();
   p.set<cts("vao")>(vao);
   p.set<cts("uPos")>(mPos);
   p.set<cts("uSize")>(mSize);
   p.set<cts("uAtlasSize")>(mAtlasSize);
   p.set<cts("uAtlasPos")>(mAtlasPos);
   p.set<cts("uTexture")>(mTexture);
   p.drawElements();
}

void CHealthBar::draw(const SContext&) const
{
   static auto vao = gRenderer.make_vao<cts("billboard-hb"),
                                        cts("aPos"), cts("indices")>(
                                           posBuffer(), indexBuffer());
   auto& p = gRenderer.get<cts("billboard-hb")>();
   p.set<cts("vao")>(vao);
   p.set<cts("uPos")>(mPos);
   p.set<cts("uSize")>(mSize);
   p.set<cts("uValue")>(mValue);

   gl(glDisable, GL_DEPTH_TEST);
   p.drawElements();
   gl(glEnable, GL_DEPTH_TEST);
}
