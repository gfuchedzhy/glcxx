/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Sphere.hpp"
#include "Programs.hpp"
#include "Context.hpp"
#include <glm/gtx/transform.hpp>
#include <vector>

namespace
{
   static std::shared_ptr<TBufferObject<glm::vec3>> buffer;
   static std::shared_ptr<TBufferObject<glm::vec3>> normalsBuffer;
   static std::vector<GLushort> indices;
   static std::vector<GLushort> normalIndices;
   void initSphere()
   {
      if (!buffer)
      {
         const size_t N = 9;
         const size_t verticesNum = 2*N*(N+1);
         const float deltaAngle = M_PI/N;

         std::vector<glm::vec3> vertices;
         // additional vertex will be (0,0,0) for normal drawing, so reserve +1
         // for it
         vertices.reserve(verticesNum + 1);
         auto ind = [N] (size_t j, size_t i) {return (j%(2*N)) + i*2*N; };

         float sinPhi[2*N], cosPhi[2*N];
         for (size_t j = 0; j < 2*N; ++j)
         {
            const float phi = j*deltaAngle;
            sinPhi[j] = sin(phi);
            cosPhi[j] = cos(phi);
         }

         for (size_t i = 0; i <= N; ++i)
         {
            const float theta = i*deltaAngle;
            const float sinTheta = sin(theta);
            const float cosTheta = cos(theta);
            for (size_t j = 0; j < 2*N; ++j)
            {
               normalIndices.push_back(verticesNum); // (0,0,0)
               normalIndices.push_back(vertices.size());
               vertices.push_back({sinTheta*cosPhi[j], sinTheta*sinPhi[j], cosTheta});
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
         buffer = std::make_shared<TBufferObject<glm::vec3>>(&vertices[0], vertices.size());

         // scale for normals
         for (auto&& v : vertices)
            v *= 1.5f;
         vertices.push_back({0, 0, 0});
         normalsBuffer = std::make_shared<TBufferObject<glm::vec3>>(&vertices[0], vertices.size());
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
   auto p = gRenderer.getAndSelect<cts("coloredIlluminated")>();
   p->set<cts("aPos")>(buffer);
   p->set<cts("aNorm")>(buffer);
   p->set<cts("uModel")>(model());
   p->set<cts("uColor")>(mColor);
   gl(glDrawElements, GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);

   if (context.mDrawNormals)
   {
      auto p = gRenderer.getAndSelect<cts("colored")>();
      p->set<cts("aPos")>(normalsBuffer);
      p->set<cts("uModel")>(model());
      p->set<cts("uColor")>({1.f, 1.f, 1.f});
      gl(glDrawElements, GL_LINE_STRIP, normalIndices.size(), GL_UNSIGNED_SHORT, &normalIndices[0]);
   }
}
