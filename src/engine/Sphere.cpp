/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Sphere.hpp"
#include <vector>

/// @brief generates indices and vertices for halfsphere
void generateHalfSphere(size_t N,
                        std::vector<glm::vec3>& vertices,
                        std::vector<glm::vec2>& texCoords,
                        std::vector<GLushort>& indices,
                        std::vector<glm::vec3>* tangents)
{
   const float deltaTheta = 0.5f*M_PI/N;

   // top point
   vertices.emplace_back(0, 0, 1);
   if (tangents)
      tangents->emplace_back(1, 0, 0);
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
      if (tangents)
         tangents->emplace_back(0, 1, 0);
      texCoords.emplace_back(0, 2*theta/M_PI);

      // j is index in current row, k is index in previous row
      for (size_t j = 1, k = 1; j <= n; ++j)
      {
         const float phi = j*deltaPhi;
         const float cosPhi = cos(phi);
         const float sinPhi = sin(phi);
         vertices.emplace_back(sinTheta*cosPhi, sinTheta*sinPhi, cosTheta);
         if (tangents)
            tangents->emplace_back(-sinPhi, cosPhi, 0);
         texCoords.emplace_back(phi/(2*M_PI), 2*theta/M_PI);

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
