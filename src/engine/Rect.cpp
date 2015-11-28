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

   const glm::vec3 posVertexData[] = {
      {-0.5f,-0.5f, 0.f},
      { 0.5f,-0.5f, 0.f},
      { 0.5f, 0.5f, 0.f},
      {-0.5f, 0.5f, 0.f}
   };

   const GLubyte indices[] = {0, 1, 3, 2};

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
   auto p = gRenderer.getAndSelect<cts("texturedPolygon")>();
   p->set<cts("aPos,aUV")>(posUVBuffer());
   p->set<cts("uModel")>(model());
   p->set<cts("uTexture")>(mTexture);

   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
}

void CTexturedBillboard::draw(const SContext&) const
{
   auto p = gRenderer.getAndSelect<cts("texturedBillboard")>();
   p->set<cts("aPos,aUV")>(posUVBuffer());
   p->set<cts("uPos")>(mPos);
   p->set<cts("uSize")>(mSize);
   p->set<cts("uTexture")>(mTexture);

   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
}

void CAnimationObject::update(const float timeDelta)
{
   static int fps = 60;
   mCurrentFrame += fps*timeDelta;
   mCurrentFrame -= mFrameNumber*int(mCurrentFrame/mFrameNumber);
}

void CAnimationObject::draw(const SContext& context) const
{
   auto p = gRenderer.getAndSelect<cts("animationObject")>();
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
   auto p = gRenderer.getAndSelect<cts("healthbar")>();
   p->set<cts("aPos")>(posBuffer());
   p->set<cts("uPos")>(mPos);
   p->set<cts("uSize")>(mSize);
   p->set<cts("uValue")>(mValue);

   gl(glDisable, GL_DEPTH_TEST);
   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
   gl(glEnable, GL_DEPTH_TEST);
}
