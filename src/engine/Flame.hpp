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

      /// @brief speed of particle source
      glm::vec3 mSpeed;

      /// @brief direction in which particles are emmited
      glm::vec3 mDir;

   public:
      /// @brief constructor
      CJetFlame(float radius);

      /// @brief set speed
      void speed(const glm::vec3& speed) { mSpeed = speed; }

      /// @brief set dir
      void dir(const glm::vec3& dir) { mDir = dir; }

      /// @brief update
      void update(float timeDelta)
      {
         tParticleSystem::update(*this, timeDelta);
      }

      /// @brief fill parameters for resurrected particle
      void resetParticle(SParticle& p, float timeDelta) const
      {
         std::uniform_real_distribution<float> distr(-0.01, 0.01);
         const glm::vec3 speed = mSpeed + 100.f * (mDir + glm::vec3{distr(random_gen), distr(random_gen), distr(random_gen)});

         // most of particles die young
         std::normal_distribution<float> tdistr(0, 0.5);

         // consider that particle was born some time ago, therefore it was born
         // not in exactly this position
         p.reset(mPos - mSpeed*timeDelta, 0.5 + 4*std::abs(tdistr(random_gen)), speed);
      }

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif
