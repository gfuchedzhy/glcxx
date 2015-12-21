/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Particle.hpp"

std::shared_ptr<CIndexBuffer> getCachedParticleIndices(size_t particleNum)
{
   static std::shared_ptr<CIndexBuffer> buf = std::make_shared<CIndexBuffer>((GLubyte*)nullptr, 0, GL_TRIANGLES);
   static size_t num = 0;

   // if current buffer fits requirements just return it
   if (particleNum <= num)
      return buf;

   static const size_t uByteNum  = (1+std::numeric_limits<GLubyte>::max())/6;
   static const size_t uShortNum = (1+std::numeric_limits<GLushort>::max())/6;
   if (particleNum <= uByteNum) // buffer fits into ubyte indices
   {
      std::vector<GLubyte> indices;
      for (GLubyte i = 0; i < uByteNum; ++i)
      {
         GLubyte n = 4*i;
         indices.insert(indices.end(), {n, GLubyte(n+1), GLubyte(n+3), GLubyte(n+3), GLubyte(n+1), GLubyte(n+2)});
      }
      buf->upload(&indices[0], indices.size(), GL_TRIANGLES);
      num = uByteNum;
   }
   else if (particleNum <= uShortNum) // buffer fits into ushort indices
   {
      std::vector<GLushort> indices;
      for (GLushort i = 0; i < particleNum; ++i)
      {
         GLushort n = 4*i;
         indices.insert(indices.end(), {n, GLushort(n+1), GLushort(n+3), GLushort(n+3), GLushort(n+1), GLushort(n+2)});
      }
      buf->upload(&indices[0], indices.size(), GL_TRIANGLES);
      num = particleNum;
   }
   else
      throw std::runtime_error{"integer indices for particle systems not supported"};
   return buf;
}
