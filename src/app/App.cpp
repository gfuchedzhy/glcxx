/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "App.hpp"
#include "Texture.hpp"

namespace
{
   std::uniform_real_distribution<float> distr(0.8f, 1.05f);
   std::uniform_real_distribution<float> distr2(-1, 1);
   const float horizonDistance = 30e3;
}

CApp::CApp()
   : CEngine(1280, 960)
   , mSky(make_texture("res/sky.dds"), horizonDistance)
   , mTerrain(make_texture("res/ground.dds"))
{
   mTerrain.scale({0.3*horizonDistance, 0.3*horizonDistance, 1.f});
   mTerrain.radius(horizonDistance);

   mAircraft.pos({0, 0, 1.5e3});

   mCamera.perspective(60, mAspect, 1, 4e4);
   mTargetCameraEyeDistance = 35;
   mCamera.eyeDistance(mTargetCameraEyeDistance);
   mTargetCameraPitch = 15;
   mCamera.pitch(mTargetCameraPitch);

   auto starTexture = make_texture("res/star-sprite.dds");
   for (auto& s: mStars)
   {
      s.atlasSize({4, 4});
      s.texture(starTexture);
      s.size({15, 15});
      s.pos({1e3*distr2(random_gen), 1e3*distr2(random_gen), 1.5e3 * distr(random_gen)});
   }
}

/// @brief return damped value
inline float damp(float value, float timeDelta, float targetValue, float dampCoef = 0.8f)
{
   return targetValue + (value - targetValue) * std::max(0.f, 1.f - timeDelta/dampCoef);
}

/// @brief animates value returned by get in given range on given keypresses
template<typename Get, typename Set>
inline bool animate(Get get, Set set,
                    float timeDelta, float speed,
                    sf::Keyboard::Key minKey, sf::Keyboard::Key maxKey,
                    float minVal, float maxVal)
{
   if (sf::Keyboard::isKeyPressed(minKey))
      set(std::max(minVal, get() - timeDelta*speed));
   else if (sf::Keyboard::isKeyPressed(maxKey))
      set(std::min(maxVal, get() + timeDelta*speed));
   else
      return false;
   return true;
}

void CApp::update(float timeDelta)
{
   mAircraft.update(timeDelta);
   if (mCameraFollowsAircraft)
      mCamera.lookAt(mAircraft.pos());

   const auto& pos = mCamera.eye();
   mSky.pos({pos.x, pos.y, mSky.pos().z});
   mTerrain.center({pos.x, pos.y});

   for (auto& s : mStars)
   {
      // @todo redo this with bounding boxes
      if (glm::distance(s.pos(), mAircraft.pos()) < 20)
      {
         Log::msg("bonus collected");
         mAircraft.repair();
         s.pos({1e3*distr2(random_gen), 1e3*distr2(random_gen), 1.5e3 * distr(random_gen)});
      }
      s.update(timeDelta);
   }

   constexpr auto inf = std::numeric_limits<float>::infinity();
   if (mIsCameraControl)
   {
      animate([this]{return mTargetCameraPitch;},
              [this](float val){mTargetCameraPitch = val;},
              timeDelta, 30.f, sf::Keyboard::Down, sf::Keyboard::Up, -inf, inf);
      animate([this]{return mTargetCameraEyeDistance;},
              [this](float val){mTargetCameraEyeDistance = val;},
              timeDelta, 30.f, sf::Keyboard::Add, sf::Keyboard::Subtract, 10.f, 500.f);
   }
   mCamera.pitch(damp(mCamera.pitch(), timeDelta, mTargetCameraPitch, 1.f));
   mCamera.eyeDistance(damp(mCamera.eyeDistance(), timeDelta, mTargetCameraEyeDistance, 1.f));

   if (mIsCameraControl)
   {
      animate([this]{return mRelativeCameraOrientation;},
              [this](float val){mRelativeCameraOrientation = val;},
              timeDelta, 60.f, sf::Keyboard::Left, sf::Keyboard::Right, -inf, inf);
   }
   mCamera.orientation(damp(mCamera.orientation(), timeDelta, mRelativeCameraOrientation + mAircraft.yaw(), 1.f));

   if (mIsCameraControl || !animate([this]{return mAircraft.roll();},
                                    [this](float val){mAircraft.roll(val);},
                                    timeDelta, 30.f, sf::Keyboard::Left, sf::Keyboard::Right, -60.f, 60.f))
   {
      mAircraft.roll(damp(mAircraft.roll(), timeDelta, 0));
   }
   const float yawSpeed = 0.5f * mAircraft.roll();
   mAircraft.yaw(mAircraft.yaw() - yawSpeed*timeDelta);

   if(0 != mAircraft.speed())
   {
      if (!animate([this]{return mAircraft.speed();},
                   [this](float val){mAircraft.speed(val);},
                   timeDelta, 20.f, sf::Keyboard::Unknown, sf::Keyboard::Space, 80.f, 180.f))
         mAircraft.speed(damp(mAircraft.speed(), timeDelta, 80));
   }

   if (mIsCameraControl || !animate([this]{return mAircraft.pitch();},
                                    [this](float val){ mAircraft.pitch(val);},
                                    timeDelta, 20.f, sf::Keyboard::Up, sf::Keyboard::Down, -inf, inf))
   {
      mAircraft.pitch(damp(mAircraft.pitch() - 360*int(round(mAircraft.pitch()/360)), timeDelta, 0, 1));
   }

   static const glm::vec3 sunDir = {-0.577, -0.577, 0.577};
   /// @todo make automatic uniforms in renderer to remove this code
   {  auto& p = mContext.getProgram<cts("regular-col")>();
      p.set<cts("uViewProj")>(mCamera.viewProj()); }
   {  auto& p = mContext.getProgram<cts("regular-tex")>();
      p.set<cts("uViewProj")>(mCamera.viewProj()); }
   {  auto& p = mContext.getProgram<cts("shaded-col")>();
      p.set<cts("uViewProj")>(mCamera.viewProj());
      p.set<cts("uSunDir")>(sunDir);
      p.set<cts("uEye")>(mCamera.eye()); }
   {  auto& p = mContext.getProgram<cts("shaded-tex")>();
      p.set<cts("uViewProj")>(mCamera.viewProj());
      p.set<cts("uSunDir")>(sunDir);
      p.set<cts("uEye")>(mCamera.eye()); }
   {  auto& p = mContext.getProgram<cts("shaded-tex-nmap")>();
      p.set<cts("uViewProj")>(mCamera.viewProj());
      p.set<cts("uSunDir")>(sunDir);
      p.set<cts("uEye")>(mCamera.eye()); }
   {  auto& p = mContext.getProgram<cts("billboard-tex")>();
      p.set<cts("uViewProj")>(mCamera.viewProj());
      p.set<cts("uUp")>({0, 0, 1});
      p.set<cts("uEyePos")>(mCamera.eye()); }
   {  auto& p = mContext.getProgram<cts("billboard-tex-sprite")>();
      p.set<cts("uViewProj")>(mCamera.viewProj());
      p.set<cts("uUp")>({0, 0, 1});
      p.set<cts("uEyePos")>(mCamera.eye()); }
   {  auto& p = mContext.getProgram<cts("billboard-hb")>();
      p.set<cts("uViewProj")>(mCamera.viewProj());
      p.set<cts("uUp")>(mCamera.up());
      p.set<cts("uEyePos")>(mCamera.eye()); }
   {  auto& p = mContext.getProgram<cts("particlesys-tex-sprite-flame")>();
      p.set<cts("uViewProj")>(mCamera.viewProj());
      p.set<cts("uPerspectiveScale")>(mCamera.perspectiveScale()); }
}

void CApp::onKeyPressed(const sf::Event::KeyEvent& keyEvent)
{
   switch (keyEvent.code)
   {
      case sf::Keyboard::Num1:
         mCameraFollowsAircraft = !mCameraFollowsAircraft;
         Log::msg("camera aircraft following turned ", Log::SOnOff(mCameraFollowsAircraft));
         break;
      case sf::Keyboard::Num2:
         mContext.mDrawHealthBars = !mContext.mDrawHealthBars;
         Log::msg("health bars turned ", Log::SOnOff(mContext.mDrawHealthBars));
         break;
      case sf::Keyboard::Num3:
         mContext.mDrawNormals = !mContext.mDrawNormals;
         Log::msg("normals turned ", Log::SOnOff(mContext.mDrawNormals));
         break;
      case sf::Keyboard::Tab:
         mIsCameraControl = !mIsCameraControl;
         Log::msg("camera control turned ", Log::SOnOff(mIsCameraControl));
         break;
      case sf::Keyboard::Return:
         Log::msg(mAircraft.launchRocket() ? "rocket launched" : "out of rockets");
         break;
      case sf::Keyboard::D:
         mAircraft.randomDamage();
         Log::msg("apply random damage to aircraft");
         break;
      default:
         break;
   }
}

void CApp::draw() const
{
   mTerrain.draw(mContext);
   mSky.draw(mContext);

   {
      SEnableBlendingGuard lock;
      for (const auto& s : mStars)
         s.draw(mContext);
   }

   mAircraft.draw(mContext);
}
