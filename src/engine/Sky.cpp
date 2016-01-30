/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Sky.hpp"
#include "Sphere.hpp"
#include "Context.hpp"

CSky::CSky(tTexturePtr tex, float horizonDistance)
   : mTexture(std::move(tex))
{
   scale({horizonDistance, horizonDistance, horizonDistance});

   std::array<tTexturePtr, 5> cloudTextures;
   for (int i = 0; i < cloudTextures.size(); ++i)
      cloudTextures[i] = make_texture(std::string("res/cloud") + std::to_string(i+1) + ".dds");

   std::uniform_real_distribution<float> sizeDistr(2000, 7000);
   std::uniform_real_distribution<float> angleDistr(0, 2*M_PI);
   std::uniform_real_distribution<float> distanceDistr(0.5*horizonDistance, 0.8*horizonDistance);
   std::uniform_real_distribution<float> heightDistr(1e3, 2e3);
   std::uniform_int_distribution<int> textureDistr(0, cloudTextures.size() - 1);

   mClouds.resize(100);
   for (auto& c : mClouds)
   {
      c.texture(cloudTextures[textureDistr(random_gen)]);
      c.size(sizeDistr(random_gen)*glm::vec2(1, 0.5));
      const float angle = angleDistr(random_gen);
      const float distance = distanceDistr(random_gen);
      c.pos(glm::vec3(distance*sin(angle), distance*cos(angle),
                      heightDistr(random_gen)));
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
   for (const auto& c : mClouds)
      c.draw(context);
}
