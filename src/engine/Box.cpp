/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Box.hpp"
#include "Programs.hpp"

namespace
{
   auto& posBuffer()
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
      static auto buffer = make_buffer(data, sizeof(data)/sizeof(data[0]));
      return buffer;
   }

   auto& indexBuffer()
   {
      static const GLubyte data[] = {0, 1, 3, 2, 7, 6, 4, 5, 0, 1,
                                     1, 5, 2, 6,
                                     6, 3, 3, 7, 0, 4};
      static auto buffer = make_indexBuffer(data, sizeof(data)/sizeof(data[0]), GL_TRIANGLE_STRIP);
      return buffer;
   }
}

void CBox::draw(const SContext& context) const
{
   static auto vao = make_vao<cts("aPos")>(indexBuffer(), posBuffer());
   auto& p = gRenderer.get<cts("regular-col")>();
   p.set<cts("uModel")>(model());
   p.set<cts("uColor")>(mColor);
   p.drawElements(*vao);
}
