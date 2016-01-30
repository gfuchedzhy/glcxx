/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Terrain.hpp"
#include "Context.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

CTerrain::CTerrain(tTexturePtr tex)
   : mTexture(std::move(tex))
{
   const glm::vec3 pos[] = {
      {-0.5f,-0.5f, 0.f},
      { 0.5f,-0.5f, 0.f},
      { 0.5f, 0.5f, 0.f},
      {-0.5f, 0.5f, 0.f}
   };
   const typename glm::vec2 uv[] = {
      {0.f, 1.f},
      {1.f, 1.f},
      {1.f, 0.f},
      {0.f, 0.f}
   };
   const GLubyte indices[] = {0, 1, 3, 2};
   mVAO.upload<cts("indices")>(indices, size(indices), GL_TRIANGLE_STRIP);
   mVAO.upload<cts("aPos")>(pos, size(pos));
   mVAO.upload<cts("aUV")>(uv, size(uv));
}

void CTerrain::draw(const CContext& context) const
{
   SDisableDepthTestGuard lock;
   auto& p = context.getProgram<cts("regular-tex")>();
   p.set<cts("uTexture")>(mTexture);

   const float size = scale().x;
   const glm::ivec2 min = glm::round((mCenter - mRadius) / size);
   const glm::ivec2 max = glm::round((mCenter + mRadius) / size);

   const glm::mat4& m = model();
   for (int shiftx = min.x; shiftx <= max.x; ++shiftx)
      for (int shifty = min.y; shifty <= max.y; ++shifty)
      {
         p.set<cts("uModel")>(glm::translate(glm::mat4{}, glm::vec3{size*shiftx, size*shifty, 0}) * m);
         p.drawElements(mVAO);
      }
}
