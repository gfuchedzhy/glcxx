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
   /// @brief generates indices and vertices for halfsphere
   void generateHalfSphere(size_t N, std::vector<glm::vec3>& vertices, std::vector<GLushort>& indices)
   {
      const float deltaTheta = 0.5f*M_PI/N;

      // top point
      vertices.push_back({0, 0, 1});
      // number of points in previous row
      size_t prevN = 1;
      for (size_t i = 1; i <= N; ++i)
      {
         const float theta = i*deltaTheta;
         const float sinTheta = sin(theta);
         const float cosTheta = cos(theta);

         // number of points in current row
         const size_t n = sinTheta*(4*N-1) + 1.5;
         const float deltaPhi = 2*M_PI/n;
         const GLushort stripStart = vertices.size();

         // j is index in current row, k is index in previous row
         for (size_t j = 0, k = 0; j < n; ++j)
         {
            const float phi = j*deltaPhi;
            vertices.push_back({sinTheta*cos(phi), sinTheta*sin(phi), cosTheta});

            if ((1 != i) && ((k+0.5f)*n <= (j+0.5f)*prevN))
            {
               indices.push_back(stripStart - prevN + k);
               indices.push_back(stripStart + j);
               k = (k+1) % prevN;
               indices.push_back(stripStart - prevN + k);
            }

            indices.push_back(stripStart + j);
            indices.push_back(stripStart + (j+1) % n);
            indices.push_back(stripStart - prevN + k);
         }
         prevN = n;
      }
   }

   static std::shared_ptr<TBufferObject<glm::vec3>> buffer;
   static std::shared_ptr<TBufferObject<glm::vec3>> normalsBuffer;
   static std::vector<GLushort> indices;
   static std::vector<GLushort> normalIndices;

   void initSphere()
   {
      if (!buffer)
      {
         const size_t N = 6;
         std::vector<glm::vec3> vertices;
         generateHalfSphere(N, vertices, indices);

         const size_t vertNum = vertices.size();
         // do not copy equator row as it is shared
         const size_t vertNumWithoutEquator = vertNum - 4*N;
         for (size_t i = 0; i < vertNumWithoutEquator; ++i)
         {
            const auto& p = vertices[i];
            vertices.emplace_back(p.x, p.y, -p.z);
         }
         const size_t indSize = indices.size();
         for (size_t i = 0; i < indSize; ++i)
         {
            // swap 2 indices of each triangle to alter orientation, this should
            // be done because mirroring over xy plane changes triangle orientation
            const size_t j = (1 == i % 3) ? i - 1 : (0 == i % 3) ? i+1 : i;

            // shift index unless it is index of equator vertex
            indices.push_back(indices[j] + (indices[j] < vertNumWithoutEquator ? vertNum : 0));
         }
         buffer = std::make_shared<TBufferObject<glm::vec3>>(&vertices[0], vertices.size());

         // indices for normals, last vertex would be (0,0,0)
         const size_t lastIndex = vertices.size();
         for (size_t i = 0; i < lastIndex; ++i)
         {
            normalIndices.push_back(lastIndex);
            normalIndices.push_back(i);
         }

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
