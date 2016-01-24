/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Rocket.hpp"
#include "GLState.hpp"

void CRocket::launch(const glm::vec3& pos, const glm::vec3& dir, float initialSpeed)
{
   mFlame.pos(pos);
   // speed of flame source
   mSpeed = initialSpeed;
   // exhaust direction is opposite to movement direction
   mFlame.dir(-dir);
   mIsActive = true;
   mExploded = false;
   mTime = 0;
   mFlame.rate(400);
   mExplosion.reset();
}

void CRocket::update(float timeDelta)
{
   if (mIsActive)
   {
      mTime += timeDelta;
      if(!mExploded)
      {
         mSpeed = std::min(500.f, mSpeed + 200*timeDelta);
         mFlame.sourceVelocity(-mSpeed*mFlame.dir());
         mFlame.pos(mFlame.pos() + mFlame.sourceVelocity()*timeDelta);
         mFlame.update(timeDelta);
         mExploded = mFlame.pos().z < 0 || mTime > 7;
      }
      else
      {
         mFlame.update(timeDelta);
         mFlame.rate(0);
         mExplosion.pos(mFlame.pos());
         mExplosion.update(timeDelta);
         mIsActive = mExplosion.cycles() == 0 || mFlame.hasAliveParticles();
      }
   }
}

void CRocket::draw(const CContext& context) const
{
   if (mIsActive)
   {
      mFlame.draw(context);
      if (mExploded && mExplosion.cycles() == 0)
      {
         SEnableBlendingGuard lock(GL_SRC_ALPHA, GL_ONE);
         mExplosion.draw(context);
      }
   }
}
