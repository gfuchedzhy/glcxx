/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Flame.hpp"

CJetFlame::CJetFlame(float radius, float rate, float particleSpeed)
   : tParticleSystem(rate, 0.6*rate)
   , mParticleSpeed(particleSpeed)
{
   size({radius, radius});
   mAtlasSize = {4, 4};
}

void CJetFlame::update(float timeDelta)
{
   tParticleSystem::update(*this, timeDelta);
   mVAO->upload<cts("aPos")>(&mPositions[0], mPositions.size(), GL_DYNAMIC_DRAW);
   mVAO->upload<cts("aSpeed")>(&mSpeeds[0], mSpeeds.size(), GL_DYNAMIC_DRAW);
   mVAO->upload<cts("aTime")>(&mTimes[0], mTimes.size(), GL_DYNAMIC_DRAW);
}

void CJetFlame::draw(const SContext& context) const
{
   if (hasAliveParticles())
   {
      auto& p = gRenderer.get<cts("particlesys-tex-sprite-flame")>();
      p.set<cts("vao")>(mVAO);
      p.set<cts("uSize")>(mSize);
      p.set<cts("uAtlasSize")>(mAtlasSize);
      p.set<cts("uTexture")>(mTexture);
      gl(glEnable, GL_BLEND);
      gl(glBlendFunc, GL_SRC_ALPHA, GL_ONE);
      glDepthMask(GL_FALSE);
      p.drawArrays(aliveParticleNum(), GL_POINTS);
      gl(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDepthMask(GL_TRUE);
      gl(glDisable, GL_BLEND);
   }
}
