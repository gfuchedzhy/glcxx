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

   std::shared_ptr<TBufferObject<tPosUVAttrib::tData>> vertexBuffer;
   void initializePosUVVertexBuffer()
   {
      if (!vertexBuffer)
      {
         vertexBuffer = std::make_shared<TBufferObject<tPosUVAttrib::tData>>();
         vertexBuffer->upload(vertexData, sizeof(vertexData)/sizeof(vertexData[0]));
      }
   }

   std::shared_ptr<TBufferObject<glm::vec3>> posVertexBuffer;
   void initializePosVertexBuffer()
   {
      if (!posVertexBuffer)
      {
         posVertexBuffer = std::make_shared<TBufferObject<glm::vec3>>();
         posVertexBuffer->upload(posVertexData, sizeof(posVertexData)/sizeof(posVertexData[0]));
      }
   }
}

CTexturedRect::CTexturedRect()
{
   initializePosUVVertexBuffer();
}

void CTexturedRect::draw(const SContext&) const
{
   auto p = gRenderer.getAndSelect<cts("texturedPolygon")>();
   p->set<cts("aPos,aUV")>(vertexBuffer);
   p->set<cts("uModel")>(model());
   p->set<cts("uTexture")>(mTexture);

   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
}

CTexturedBillboard::CTexturedBillboard()
{
   initializePosUVVertexBuffer();
}

void CTexturedBillboard::draw(const SContext&) const
{
   auto p = gRenderer.getAndSelect<cts("texturedBillboard")>();
   p->set<cts("aPos,aUV")>(vertexBuffer);
   p->set<cts("uPos")>(mPos);
   p->set<cts("uSize")>(mSize);
   p->set<cts("uTexture")>(mTexture);

   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
}

CHealthBar::CHealthBar(float value)
   : mValue(value)
{
   initializePosVertexBuffer();
}

void CHealthBar::draw(const SContext&) const
{
   auto p = gRenderer.getAndSelect<cts("healthbar")>();
   p->set<cts("aPos")>(posVertexBuffer);
   p->set<cts("uPos")>(mPos);
   p->set<cts("uSize")>(mSize);
   p->set<cts("uValue")>(mValue);

   gl(glDisable, GL_DEPTH_TEST);
   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
   gl(glEnable, GL_DEPTH_TEST);
}
