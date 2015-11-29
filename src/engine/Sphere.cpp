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
   void generateHalfSphere(size_t N,
                           std::vector<glm::vec3>& vertices,
                           std::vector<glm::vec2>& texCoords,
                           std::vector<GLushort>& indices)
   {
      const float deltaTheta = 0.5f*M_PI/N;

      // top point
      vertices.emplace_back(0, 0, 1);
      texCoords.emplace_back(0.5, 0);
      // number of gaps in previous row
      size_t prevN = 0;
      for (size_t i = 1; i <= N; ++i)
      {
         const float theta = i*deltaTheta;
         const float sinTheta = sin(theta);
         const float cosTheta = cos(theta);

         // number of gaps in current row
         const size_t n = 4*N*sinTheta + 0.5;
         const float deltaPhi = 2*M_PI/n;
         const GLushort stripStart = vertices.size();

         vertices.emplace_back(sinTheta, 0, cosTheta);
         texCoords.emplace_back(0, theta/M_PI);

         // j is index in current row, k is index in previous row
         for (size_t j = 1, k = 1; j <= n; ++j)
         {
            const float phi = j*deltaPhi;
            vertices.emplace_back(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
            texCoords.emplace_back(phi/(2*M_PI), theta/M_PI);

            indices.push_back(stripStart + j - 1);
            indices.push_back(stripStart + j);
            indices.push_back(stripStart - (prevN+1) + k-1);

            // average of k-1 and k angle <= average of j and j+1 angle
            if ((k-0.5f)*n <= (j+0.5f)*prevN)
            {
               indices.push_back(stripStart - (prevN+1) + k-1);
               indices.push_back(stripStart + j);
               indices.push_back(stripStart - (prevN+1) + k);
               k++;
            }
         }
         prevN = n;
      }
   }

   static std::shared_ptr<TBufferObject<glm::vec3>> buffer;
   static std::shared_ptr<TBufferObject<glm::vec2>> texBuffer;
   static std::shared_ptr<TBufferObject<glm::vec3>> normalsBuffer;
   static std::vector<GLushort> indices;
   static std::vector<GLushort> normalIndices;

   void initSphere()
   {
      if (!buffer)
      {
         const size_t N = 6;
         std::vector<glm::vec3> vertices;
         std::vector<glm::vec2> texCoords;
         generateHalfSphere(N, vertices, texCoords, indices);

         const size_t vertNum = vertices.size();
         // do not copy equator row as it is shared
         const size_t vertNumWithoutEquator = vertNum - 4*N;
         for (size_t i = 0; i < vertNumWithoutEquator; ++i)
         {
            const auto& p = vertices[i];
            vertices.emplace_back(p.x, p.y, -p.z);
            const auto& t = texCoords[i];
            texCoords.emplace_back(t.x, 1 - t.y);
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
         texBuffer = std::make_shared<TBufferObject<glm::vec2>>(&texCoords[0], texCoords.size());

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
         vertices.emplace_back(0, 0, 0);
         normalsBuffer = std::make_shared<TBufferObject<glm::vec3>>(&vertices[0], vertices.size());
      }
   }
}

CSphere::CSphere(const glm::vec3& color)
   : mColor(color)
{
   initSphere();
}

CTexturedSphere::CTexturedSphere()
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

void CTexturedSphere::draw(const SContext& context) const
{
   auto p = gRenderer.getAndSelect<cts("texturedIlluminated")>();
   p->set<cts("aPos")>(buffer);
   p->set<cts("aNorm")>(buffer);
   p->set<cts("aUV")>(texBuffer);
   p->set<cts("uModel")>(model());
   p->set<cts("uTexture")>(mTexture);
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
