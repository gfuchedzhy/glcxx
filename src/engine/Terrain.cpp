/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Terrain.hpp"
#include "Context.hpp"


CTerrain::CTerrain(std::shared_ptr<CTexture> tex)
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
   p.set<cts("uModel")>(model());
   p.set<cts("uTexture")>(mTexture);
   p.drawElements(mVAO);
}
