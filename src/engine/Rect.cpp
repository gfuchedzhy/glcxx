/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Rect.hpp"
#include "Programs.hpp"

namespace
{
   /// @note flip of Y coordinates, because loaded data are top-row-first, but
   /// opengl expects bottom-row-first
   const typename tPosUVAttrib::tData vertexData[] = {
      { {-0.5f,-0.5f, 0.f}, {0.f, 1.f} },
      { { 0.5f,-0.5f, 0.f}, {1.f, 1.f} },
      { { 0.5f, 0.5f, 0.f}, {1.f, 0.f} },
      { {-0.5f, 0.5f, 0.f}, {0.f, 0.f} }
   };

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

   auto posUVBuffer()
   {
      static auto buffer = std::make_shared<TBufferObject<tPosUVAttrib::tData>>(vertexData, sizeof(vertexData)/sizeof(vertexData[0]));
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
   auto p = gRenderer.get<cts("texturedPolygon")>();
   p->set<cts("aPos")>(posBuffer());
   p->set<cts("aUV")>(uvBuffer());
   p->set<cts("uModel")>(model());
   p->set<cts("uTexture")>(mTexture);

   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
}

void CTexturedBillboard::draw(const SContext&) const
{
   auto p = gRenderer.get<cts("texturedBillboard")>();
   p->set<cts("aPos,aUV")>(posUVBuffer());
   p->set<cts("uPos")>(mPos);
   p->set<cts("uSize")>(mSize);
   p->set<cts("uTexture")>(mTexture);

   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
}

void CAnimatedBillboard::draw(const SContext& context) const
{
   auto p = gRenderer.get<cts("animationObject")>();
   p->set<cts("aPos,aUV")>(posUVBuffer());
   p->set<cts("uPos")>(mPos);
   p->set<cts("uSize")>(mSize);
   p->set<cts("uFrameNumber")>(mFrameNumber);
   p->set<cts("uCurrentFrame")>(mCurrentFrame);
   p->set<cts("uTexture")>(mTexture);

   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
}

void CHealthBar::draw(const SContext&) const
{
   auto p = gRenderer.get<cts("healthbar")>();
   p->set<cts("aPos")>(posBuffer());
   p->set<cts("uPos")>(mPos);
   p->set<cts("uSize")>(mSize);
   p->set<cts("uValue")>(mValue);

   gl(glDisable, GL_DEPTH_TEST);
   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
   gl(glEnable, GL_DEPTH_TEST);
}
