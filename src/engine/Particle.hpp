/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PARTICLE_HPP
#define ENGINE_PARTICLE_HPP

#include "BufferObject.hpp"
#include <algorithm>
#include <memory>

/// @brief particle
struct SParticle
{
      /// @brief cached flag indicating if this particle is alive or not
      bool mIsAlive = false;

      /// @brief particle internal clock, 0 is birth
      float mTime;

      /// @brief particle age of death
      float mLifeTime;

      /// @brief position
      glm::vec3 mPos;

      /// @brief speed, for optimization purpose xyz should be unit direction, w
      /// scalar speed in m/s
      glm::vec4 mSpeed;

      /// @brief resurrect particle
      void reset(const glm::vec3& pos, float lifeTime, const glm::vec3& speed)
      {
         mIsAlive = true;
         mPos = pos;
         mTime = 0;
         mLifeTime = lifeTime;
         mSpeed = glm::vec4(glm::normalize(speed), glm::length(speed));
      }

      /// @brief update with time interval
      void update(float timeDelta)
      {
         mTime += timeDelta;
         mIsAlive = mTime < mLifeTime;
      }
};

/// @brief returns cached index buffer that has indices for particleNum or more
/// particles
std::shared_ptr<CIndexBuffer> getCachedParticleIndices(size_t particleNum);

/// @brief particle system
template<typename TParticle, typename TParticleManager>
class TParticleSystem
{
   protected:
      /// @brief birth rate in units per second
      float mRate;

      /// @brief how many particles should be created since last update, @note
      /// that it is float to accumulate particle number correctly
      float mParticlesToCreate = 0;

      /// @brief particle pool
      using tParticlePool = std::vector<TParticle>;
      tParticlePool mParticlePool;

      /// @brief position in particle pool for allocation purposes
      typename tParticlePool::iterator mPoolPos;

      /// @brief positions
      std::vector<glm::vec3> mPositions;

      /// @brief position buffer
      std::shared_ptr<TBufferObject<glm::vec3>> mPosBuffer = std::make_shared<TBufferObject<glm::vec3>>(nullptr, 0, GL_DYNAMIC_DRAW);

      /// @brief speed
      std::vector<glm::vec4> mSpeeds;

      /// @brief speed buffer
      std::shared_ptr<TBufferObject<glm::vec4>> mSpeedBuffer = std::make_shared<TBufferObject<glm::vec4>>(nullptr, 0, GL_DYNAMIC_DRAW);

      /// @brief time parameters
      std::vector<glm::vec2> mTimes;

      /// @brief time buffer
      std::shared_ptr<TBufferObject<glm::vec2>> mTimeBuf = std::make_shared<TBufferObject<glm::vec2>>(nullptr, 0, GL_DYNAMIC_DRAW);

      /// @brief number of alive particles
      unsigned int mAliveParticleNum = 0;

      /// @brief index buffer containing indices for at least mAliveParticleNum
      /// particles
      std::shared_ptr<CIndexBuffer> mInd;

   public:
      /// @brief constructor
      TParticleSystem(float rate, int maxParticleNumHint)
         : mRate(rate)
      {
         mParticlePool.resize(maxParticleNumHint);
         mPositions.reserve(maxParticleNumHint);
         mSpeeds.reserve(maxParticleNumHint);
         mTimes.reserve(maxParticleNumHint);
         mPoolPos = begin(mParticlePool);
      }

      /// @brief set rate
      void rate(float rate) { mRate = rate; }

      /// @brief return number of currently alive particles
      unsigned int aliveParticleNum() const { return mAliveParticleNum; }

      /// @brief update particle system
      void update(TParticleManager& particleMgr, float timeDelta)
      {
         for (auto& p : mParticlePool)
            if (p.mIsAlive)
               updateParticle(p, timeDelta);

         mParticlesToCreate += mRate*timeDelta;
         for (; mParticlesToCreate >= 1; --mParticlesToCreate)
         {
            // how long ago particle was created
            const float t = mParticlesToCreate/mRate;
            TParticle& p = allocate();
            particleMgr.resetParticle(p, t);
            if (p.mIsAlive)
               updateParticle(p, t);
         }

         /// upload to gpu
         mPosBuffer->upload(&mPositions[0], mPositions.size());
         mSpeedBuffer->upload(&mSpeeds[0], mSpeeds.size());
         mTimeBuf->upload(&mTimes[0], mTimes.size());

         mAliveParticleNum = mPositions.size()/4;
         mInd = getCachedParticleIndices(mAliveParticleNum);

         /// clear, do not shrink to avoid reallocations on next update
         mPositions.clear();
         mSpeeds.clear();
         mTimes.clear();
      }

   private:
      /// @brief update particle
      void updateParticle(TParticle& p, float timeDelta)
      {
         p.update(timeDelta);
         if (p.mIsAlive)
         {
            mTimes.insert(end(mTimes), 4, {p.mTime, p.mLifeTime});
            mPositions.insert(end(mPositions), 4, p.mPos);
            mSpeeds.insert(end(mSpeeds), 4, p.mSpeed);
         }
      }

      /// @brief allocates particle from pool
      TParticle& allocate()
      {
         auto pred = [](const TParticle& x) { return !x.mIsAlive; };
         const auto pos = mPoolPos;
         // find first dead particle starting from mPoolPos
         mPoolPos = std::find_if(mPoolPos, end(mParticlePool), pred);
         if (end(mParticlePool) == mPoolPos)
         {
            mPoolPos = std::find_if(begin(mParticlePool), pos, pred);
            if (pos == mPoolPos)
            {
               // let container decide by how much its capacity will be
               // increased
               mParticlePool.emplace_back();
               mPoolPos = end(mParticlePool) - 1;
               // no reallocation, iterator remains valid
               mParticlePool.resize(mParticlePool.capacity());
            }
         }
         return *mPoolPos;
      }
};

#endif
