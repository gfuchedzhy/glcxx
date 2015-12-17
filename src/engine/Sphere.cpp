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
                           std::vector<glm::vec3>& tangents,
                           std::vector<glm::vec2>& texCoords,
                           std::vector<GLushort>& indices)
   {
      const float deltaTheta = 0.5f*M_PI/N;

      // top point
      vertices.emplace_back(0, 0, 1);
      tangents.emplace_back(1, 0, 0);
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
         tangents.emplace_back(0, 1, 0);
         texCoords.emplace_back(0, theta/M_PI);

         // j is index in current row, k is index in previous row
         for (size_t j = 1, k = 1; j <= n; ++j)
         {
            const float phi = j*deltaPhi;
            const float cosPhi = cos(phi);
            const float sinPhi = sin(phi);
            vertices.emplace_back(sinTheta*cosPhi, sinTheta*sinPhi, cosTheta);
            tangents.emplace_back(-sinPhi, cosPhi, 0);
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
   static std::shared_ptr<TBufferObject<glm::vec3>> tanBuffer;
   static std::shared_ptr<TBufferObject<glm::vec3>> normalBuffer;
   static std::shared_ptr<CIndexBuffer> indexBuffer;
   static std::shared_ptr<CIndexBuffer> normalIndexBuffer;

   void initSphere()
   {
      if (!buffer)
      {
         const size_t N = 6;
         std::vector<glm::vec3> vertices;
         std::vector<glm::vec3> tangents;
         std::vector<glm::vec2> texCoords;
         std::vector<GLushort> indices;
         std::vector<GLushort> normalIndices;
         generateHalfSphere(N, vertices, tangents, texCoords, indices);

         const size_t vertNum = vertices.size();
         // do not copy equator row as it is shared
         const size_t vertNumWithoutEquator = vertNum - 4*N;
         for (size_t i = 0; i < vertNumWithoutEquator; ++i)
         {
            const auto& p = vertices[i];
            vertices.emplace_back(p.x, p.y, -p.z);
            tangents.push_back(tangents[i]);
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
         tanBuffer = std::make_shared<TBufferObject<glm::vec3>>(&tangents[0], tangents.size());
         indexBuffer = std::make_shared<CIndexBuffer>(&indices[0], indices.size(), GL_TRIANGLES);

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
         normalBuffer = std::make_shared<TBufferObject<glm::vec3>>(&vertices[0], vertices.size());
         normalIndexBuffer = std::make_shared<CIndexBuffer>(&normalIndices[0], normalIndices.size(), GL_LINE_STRIP);
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
   auto p = gRenderer.get<cts("shaded-col")>();
   p->set<cts("aPos")>(buffer);
   p->set<cts("aNorm")>(buffer);
   p->set<cts("uModel")>(model());
   p->set<cts("uColor")>(mColor);
   p->set<cts("uAmbient")>({1, 1, 1});
   p->set<cts("uDiffuse")>({1, 1, 1});
   p->set<cts("uSpecular")>({1, 1, 1});
   p->set<cts("uShininess")>(20);
   p->draw(indexBuffer);

   if (context.mDrawNormals)
   {
      auto p = gRenderer.get<cts("regular-col")>();
      p->set<cts("aPos")>(normalBuffer);
      p->set<cts("uModel")>(model());
      p->set<cts("uColor")>({1.f, 1.f, 1.f});
      p->draw(normalIndexBuffer);
   }
}

void CTexturedSphere::draw(const SContext& context) const
{
   auto p = gRenderer.get<cts("shaded-tex-nmap")>();
   p->set<cts("aPos")>(buffer);
   p->set<cts("aUV")>(texBuffer);
   p->set<cts("aNorm")>(buffer);
   p->set<cts("aTan")>(tanBuffer);
   p->set<cts("uModel")>(model());
   p->set<cts("uTexture")>(mTexture);
   p->set<cts("uNormalMap")>(mNormalMap);
   p->set<cts("uAmbient")>({0.2, 0.2, 0.2});
   p->set<cts("uDiffuse")>({0.5, 0.5, 0.5});
   p->set<cts("uSpecular")>({1, 1, 1});
   p->set<cts("uShininess")>(20);
   p->draw(indexBuffer);

   if (context.mDrawNormals)
   {
      auto p = gRenderer.get<cts("regular-col")>();
      p->set<cts("aPos")>(normalBuffer);
      p->set<cts("uModel")>(model());
      p->set<cts("uColor")>({1.f, 1.f, 1.f});
      p->draw(normalIndexBuffer);
   }
}
