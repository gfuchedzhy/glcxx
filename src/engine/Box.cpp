/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Box.hpp"
#include "Programs.hpp"

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

   std::shared_ptr<TBufferObject<glm::vec3>> vertexBuffer;
}

CBox::CBox(const glm::vec3& color)
   : mColor(color)
{
   if (!vertexBuffer)
   {
      vertexBuffer = std::make_shared<TBufferObject<glm::vec3>>();
      vertexBuffer->upload(vertexData, sizeof(vertexData)/sizeof(vertexData[0]));
   }
}

void CBox::draw() const
{
   auto p = gRenderer.getAndSelect<cts("coloredPolygon")>();
   p->set<cts("aPos")>(vertexBuffer);
   p->set<cts("uModel")>(model());
   p->set<cts("uColor")>(mColor);
   gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
}
