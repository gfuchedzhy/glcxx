/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Flame.hpp"
#include "GLState.hpp"

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
   mVAO.upload<cts("aPos")>(&mPositions[0], mPositions.size(), GL_STREAM_DRAW);
   mVAO.upload<cts("aSpeed")>(&mSpeeds[0], mSpeeds.size(), GL_STREAM_DRAW);
   mVAO.upload<cts("aTime")>(&mTimes[0], mTimes.size(), GL_STREAM_DRAW);
}

void CJetFlame::draw(const SContext& context) const
{
   if (hasAliveParticles())
   {
      auto& p = gRenderer.get<cts("particlesys-tex-sprite-flame")>();
      p.set<cts("uSize")>(mSize);
      p.set<cts("uAtlasSize")>(mAtlasSize);
      p.set<cts("uTexture")>(mTexture);
      SDisableDepthMaskGuard dtLock;
      SEnableBlendingGuard bLock(GL_SRC_ALPHA, GL_ONE);
      p.drawArrays(mVAO, aliveParticleNum(), GL_POINTS);
   }
}
