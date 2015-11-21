/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Sphere.hpp"
#include "Programs.hpp"
#include <vector>

namespace
{
   static std::shared_ptr<TBufferObject<glm::vec3>> buffer;
   static std::vector<GLushort> indices;
   void initSphere()
   {
      if (!buffer)
      {
         std::vector<glm::vec3> vertices;
         const size_t N = 9;
         const float deltaAngle = M_PI/N;
         auto ind = [N] (size_t j, size_t i) {return (j%(2*N)) + i*2*N; };
         for (size_t i = 0; i <= N; ++i)
         {
            const float theta = i*deltaAngle;
            const float sinTheta = sin(theta);
            const float cosTheta = cos(theta);
            for (size_t j = 0; j < 2*N; ++j)
            {
               const float phi = j*deltaAngle;
               vertices.push_back({sinTheta*cos(phi), sinTheta*sin(phi), cosTheta});
               if (0 != i)
               {
                  indices.push_back(ind(j, i));
                  indices.push_back(ind(j+1, i-1));
                  indices.push_back(ind(j, i-1));

                  indices.push_back(ind(j, i));
                  indices.push_back(ind(j+1, i));
                  indices.push_back(ind(j+1, i-1));
               }
            }
         }
         Log::msg(vertices.size());
         Log::msg(indices.size());
         buffer = std::make_shared<TBufferObject<glm::vec3>>(&vertices[0], vertices.size());
      }
   }
}

CSphere::CSphere(const glm::vec3& color)
   : mColor(color)
{
   initSphere();
}

void CSphere::draw(const SContext& context) const
{
   auto p = gRenderer.getAndSelect<cts("colored")>();
   p->set<cts("aPos")>(buffer);
   p->set<cts("uModel")>(model());
   p->set<cts("uColor")>(mColor);
   gl(glDrawElements, GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
}
