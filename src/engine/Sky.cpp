/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Sky.hpp"
#include "Sphere.hpp"
#include "Context.hpp"

CSky::CSky(tTexturePtr tex)
   : mTexture(std::move(tex))
{
   std::uniform_real_distribution<float> distr1(0.8f, 1.5f);
   std::uniform_real_distribution<float> distr2(-0.1f, 0.f);
   float angle = 0;
   const float bbRadius = 3e3;
   auto cloudTexture = std::make_shared<CTexture>("res/cloud.dds");
   for (auto& c : mClouds)
   {
      c.texture(cloudTexture);
      c.size(glm::vec2(500*distr1(random_gen), 350*distr1(random_gen)));
      const float jitteredAngle = distr1(random_gen)*angle;
      c.pos(glm::vec3(bbRadius*sin(jitteredAngle), bbRadius*cos(jitteredAngle), 15e2 + // clouds around 1km height
                      bbRadius*distr2(random_gen)));
      angle += 2*M_PI/mClouds.size();
   }

   std::vector<glm::vec3> vertices;
   std::vector<glm::vec2> texCoords;
   std::vector<GLushort> indices;
   generateHalfSphere(7, vertices, texCoords, indices);
   mVAO.upload<cts("indices")>(indices.data(), indices.size(), GL_TRIANGLES);
   mVAO.upload<cts("aPos")>(vertices.data(), vertices.size());
   mVAO.upload<cts("aUV")>(texCoords.data(), texCoords.size());
}

void CSky::draw(const CContext& context) const
{
   {
      SDisableDepthTestGuard lock;
      auto& p = context.getProgram<cts("regular-tex")>();
      p.set<cts("uModel")>(model());
      p.set<cts("uTexture")>(mTexture);
      p.drawElements(mVAO);
   }

   SDisableDepthMaskGuard dmLock;
   SEnableBlendingGuard bLock;
   // @todo this is a hack, should rethink model of complex objects
   auto& p = context.getProgram<cts("billboard-tex")>();
   p.set<cts("uExternalPos")>({pos().x, pos().y, 0});
   for (const auto& c : mClouds)
   {
      c.draw(context);
   }
   p.set<cts("uExternalPos")>({0, 0, 0});
}
