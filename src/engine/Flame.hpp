/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_FLAME_HPP
#define ENGINE_FLAME_HPP

#include "Renderable.hpp"
#include "Rect.hpp"
#include "Particle.hpp"

/// @brief jet flame with trail
class CJetFlame : public CTexturedBillboard, public TParticleSystem<SParticle, CJetFlame>
{
      /// @brief base class
      using tParticleSystem = TParticleSystem<SParticle, CJetFlame>;

      /// @brief atlas size
      glm::ivec2 mAtlasSize;

      /// @brief velocity of particle source
      glm::vec3 mSourceVelocity;

      /// @brief initial particle speed
      float mParticleSpeed;

      /// @brief direction in which particles are emmited
      glm::vec3 mParticleSpeedDir;

   public:
      /// @brief constructor
      CJetFlame(float radius, float rate, float particleSpeed);

      /// @brief get speed
      const glm::vec3& sourceVelocity() const { return mSourceVelocity; }

      /// @brief set speed
      void sourceVelocity(const glm::vec3& velocity) { mSourceVelocity = velocity; }

      /// @brief get dir
      const glm::vec3& dir() const { return mParticleSpeedDir; }

      /// @brief set dir
      void dir(const glm::vec3& dir) { mParticleSpeedDir = dir; }

      /// @brief update
      void update(float timeDelta)
      {
         tParticleSystem::update(*this, timeDelta);
      }

      /// @brief fill parameters for resurrected particle
      void resetParticle(SParticle& p, float timeDelta) const
      {
         std::uniform_real_distribution<float> distr(-0.01, 0.01);
         const glm::vec3 speed = mSourceVelocity + mParticleSpeed * (mParticleSpeedDir + glm::vec3{distr(random_gen), distr(random_gen), distr(random_gen)});

         // most of particles die young
         std::normal_distribution<float> tdistr(0, 0.5);

         // consider that particle was born some time ago, therefore it was born
         // not in exactly this position
         p.reset(mPos - mSourceVelocity*timeDelta, 0.5 + 4*std::abs(tdistr(random_gen)), speed);
      }

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif
