/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Box.hpp"
#include "Programs.hpp"
#include "Context.hpp"

namespace
{
   auto vertexBuffer()
   {
      static const typename glm::vec3 data[] = {
         {-0.5f,-0.5f, 0.5f},
         { 0.5f,-0.5f, 0.5f},
         { 0.5f, 0.5f, 0.5f},
         {-0.5f, 0.5f, 0.5f},
         {-0.5f,-0.5f,-0.5f},
         { 0.5f,-0.5f,-0.5f},
         { 0.5f, 0.5f,-0.5f},
         {-0.5f, 0.5f,-0.5f}
      };
      static auto buffer = std::make_shared<TBufferObject<glm::vec3>>(data, sizeof(data)/sizeof(data[0]));
      return buffer;
   }

   auto indexBuffer()
   {
      const GLubyte data[] = {0, 1, 3, 2, 7, 6, 4, 5, 0, 1,
                                 1, 5, 2, 6,
                                 6, 3, 3, 7, 0, 4};
      static auto buffer = std::make_shared<CIndexBuffer>(data, sizeof(data)/sizeof(data[0]), GL_TRIANGLE_STRIP);
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
   auto p = gRenderer.get<cts("regular-col")>();
   p->set<cts("aPos")>(vertexBuffer());
   p->set<cts("uModel")>(model());
   p->set<cts("uColor")>(mColor);
   p->draw(indexBuffer());

   if (context.mDrawDebugFrames)
   {
      glLineWidth(2.f);
      p->set<cts("aPos")>(axesVertexBuffer());
      for (int i = 0; i < 3; ++i)
      {
         p->set<cts("uColor")>(axesVertexData[axesIndices[2*i+1]]);
         p->draw(axesIndices + 2*i, 2, GL_LINES);
      }
   }
}
