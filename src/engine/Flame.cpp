/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Flame.hpp"
#include "Programs.hpp"

CJetFlame::CJetFlame(float radius, float rate, float particleSpeed)
   : tParticleSystem(rate, 0.6*rate)
   , mParticleSpeed(particleSpeed)
{
   size({radius, radius});
   mAtlasSize = {4, 4};
}

void CJetFlame::draw(const SContext& context) const
{
   if (hasAliveParticles())
   {
      auto& p = gRenderer.get<cts("particlesys-tex-sprite-flame")>();
      p.set<cts("aPos")>(mPosBuffer);
      p.set<cts("aSpeed")>(mSpeedBuffer);
      p.set<cts("aTime")>(mTimeBuf);
      p.set<cts("uSize")>(mSize);
      p.set<cts("uAtlasSize")>(mAtlasSize);
      p.set<cts("uTexture")>(mTexture);
      gl(glEnable, GL_BLEND);
      gl(glBlendFunc, GL_SRC_ALPHA, GL_ONE);
      glDepthMask(GL_FALSE);
      gl(glDrawArrays, GL_POINTS, 0, aliveParticleNum());
      gl(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDepthMask(GL_TRUE);
      gl(glDisable, GL_BLEND);
   }
}
