/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Box.hpp"
#include "Programs.hpp"
#include "Context.hpp"

namespace
{
   const typename glm::vec3 vertexData[] = {
      {-0.5f,-0.5f, 0.5f},
      { 0.5f,-0.5f, 0.5f},
      { 0.5f, 0.5f, 0.5f},
      {-0.5f, 0.5f, 0.5f},
      {-0.5f,-0.5f,-0.5f},
      { 0.5f,-0.5f,-0.5f},
      { 0.5f, 0.5f,-0.5f},
      {-0.5f, 0.5f,-0.5f}
   };
   const GLubyte indices[] = {0, 1, 3, 2, 7, 6, 4, 5, 0, 1,
                              1, 5, 2, 6,
                              6, 3, 3, 7, 0, 4};
   auto vertexBuffer()
   {
      static auto buffer = std::make_shared<TBufferObject<glm::vec3>>(vertexData, sizeof(vertexData)/sizeof(vertexData[0]));
      return buffer;
   }

   const typename glm::vec3 axesVertexData[] = {
      { 0.0f, 0.0f, 0.0f},
      { 1.0f, 0.0f, 0.0f},
      { 0.0f, 1.0f, 0.0f},
      { 0.0f, 0.0f, 1.0f},
   };
   const GLubyte axesIndices[] = {0, 1, 0, 2, 0, 3};
   auto axesVertexBuffer()
   {
      static auto buffer = std::make_shared<TBufferObject<glm::vec3>>(axesVertexData, sizeof(axesVertexData)/sizeof(axesVertexData[0]));
      return buffer;
   }
}

void CBox::draw(const SContext& context) const
{
   auto p = gRenderer.get<cts("colored")>();
   p->set<cts("aPos")>(vertexBuffer());
   p->set<cts("uModel")>(model());
   p->set<cts("uColor")>(mColor);
   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);

   if (context.mDrawDebugFrames)
   {
      glLineWidth(2.f);
      p->set<cts("aPos")>(axesVertexBuffer());
      for (int i = 0; i < 3; ++i)
      {
         p->set<cts("uColor")>(axesVertexData[axesIndices[2*i+1]]);
         gl(glDrawElements, GL_LINES, 2, GL_UNSIGNED_BYTE, axesIndices + 2*i);
      }
   }
}
