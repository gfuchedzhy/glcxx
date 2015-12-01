/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "App.hpp"
#include "Programs.hpp"
#include "Texture.hpp"

namespace
{
   std::uniform_real_distribution<> distr(0.8f, 1.05f);
}

CApp::CApp()
   : CEngine(800, 600)
   , mAnimationObjects({32, 32})
{
   mAircraft.pos({0, 0, 1.5e3});

   mSphere.scale({5, 5, 5});
   mSphere.pos(mAircraft.pos());
   mSphere.texture(std::make_shared<CTexture>("res/earth-daymap.dds"));
   mSphere.normalMap(std::make_shared<CTexture>("res/earth-normalmap.dds"));

   mCamera.perspective(35, mAspect, 10, 2e4);
   mCamera.eyeDistance(150);
   mCamera.pitch(20);

   auto aoTexture = std::make_shared<CTexture>("res/star-sprite.dds");
   for (size_t i = 0; i < mAnimationObjects.size(); ++i)
   {
      mAnimationObjects[i].texture(aoTexture);
      mAnimationObjects[i].size({10, 10});
      mAnimationObjects[i].pos({0, 500*(i+1), 1.5e3 * distr(random_gen)});
   }
}

/// @brief performs dump animation of value returned by get to given targetValue
template<typename Get, typename Set>
inline void dumpAnimation(Get get, Set set,
                          float timeDelta, float targetValue)
{
   if (abs(get() - targetValue) > 0.001f) /// @todo there should be a better way
      set(targetValue + (get() - targetValue) * std::max(0.f, 1.f - timeDelta/0.2f));
   else
      set(targetValue);
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
   const auto& pos = mAircraft.pos();
   mSky.pos({pos.x, pos.y, mSky.pos().z});
   mTerrain.pos({pos.x, pos.y, 0});

   for (auto&& ao : mAnimationObjects)
   {
      // @todo redo this with bounding boxes
      if (glm::distance(ao.pos(), mAircraft.pos()) < 10)
      {
         mAircraft.repair();
         ao.pos({0, mAircraft.pos().y + 1e3, 1.5e3 * distr(random_gen)});
      }
      else if(ao.pos().y + 500 < mAircraft.pos().y)
         ao.pos({0, mAircraft.pos().y + 1.5e3, 1.5e3 * distr(random_gen)});
      ao.update(timeDelta);
   }

   mCamera.lookAt(mAircraft.pos());

   static const float angularSpeed = 10;
   float angle = mSphere.yaw() + timeDelta*angularSpeed;
   angle = angle - (int(angle)/360)*360;
   mSphere.yaw(angle);

   if(0 != mAircraft.speed())
   {
      if (!animate([this]{return mAircraft.speed();},
                   [this](float val){mAircraft.speed(val);},
                   timeDelta, 100.f, sf::Keyboard::Unknown, sf::Keyboard::Space, 80.f, 180.f))
         dumpAnimation([this]{return mAircraft.speed();},
                       [this](float val){mAircraft.speed(val);},
                       timeDelta, 80.f);
   }

   bool aircraftPitchChanged = false;
   bool aircraftRollChanged = false;
   constexpr auto inf = std::numeric_limits<float>::infinity();
   if (mIsCameraControl)
   {
      animate([this]{return mCamera.orientation();},
              [this](float val){mCamera.orientation(val);},
              timeDelta, 70.f, sf::Keyboard::Left, sf::Keyboard::Right, -inf, inf);
      animate([this]{return mCamera.pitch();},
              [this](float val){mCamera.pitch(val);},
              timeDelta, 70.f, sf::Keyboard::Down, sf::Keyboard::Up, -inf, inf);
      animate([this]{return mCamera.eyeDistance();},
              [this](float val){mCamera.eyeDistance(val);},
              timeDelta, 70.f, sf::Keyboard::Add, sf::Keyboard::Subtract, 20.f, 500.f);
   }
   else
   {
      float pitch;
      aircraftPitchChanged = animate([this]{return mAircraft.pitch();},
                                     [&pitch](float val){ pitch = val;},
                                     timeDelta, 70.f, sf::Keyboard::Up, sf::Keyboard::Down, -60.f, 60.f);
      if (aircraftPitchChanged && (pitch <= 0 || mAircraft.speed() > 130.f))
         mAircraft.pitch(pitch);
      else
         aircraftPitchChanged = false;
      aircraftRollChanged = animate([this]{return mAircraft.roll();},
                                    [this](float val){mAircraft.roll(val);},
                                    timeDelta, 70.f, sf::Keyboard::Left, sf::Keyboard::Right, -60.f, 60.f);
   }

   if (!aircraftPitchChanged)
      dumpAnimation([this]{return mAircraft.pitch();},
                    [this](float val){mAircraft.pitch(val);},
                    timeDelta, 0);
   if (!aircraftRollChanged)
      dumpAnimation([this]{return mAircraft.roll();},
                    [this](float val){mAircraft.roll(val);},
                    timeDelta, 0);

   /// @todo make automatic uniforms in renderer to remove this code
   auto p = gRenderer.get<cts("colored")>();
   p->set<cts("uViewProj")>(mCamera.viewProj());
   auto p2 = gRenderer.get<cts("texturedPolygon")>();
   p2->set<cts("uViewProj")>(mCamera.viewProj());
   auto p3 = gRenderer.get<cts("texturedBillboard")>();
   p3->set<cts("uViewProj")>(mCamera.viewProj());
   p3->set<cts("uUp")>(mCamera.up());
   p3->set<cts("uRight")>(mCamera.right());
   auto p4 = gRenderer.get<cts("healthbar")>();
   p4->set<cts("uViewProj")>(mCamera.viewProj());
   p4->set<cts("uUp")>(mCamera.up());
   p4->set<cts("uRight")>(mCamera.right());
   auto p5 = gRenderer.get<cts("coloredIlluminated")>();
   p5->set<cts("uViewProj")>(mCamera.viewProj());
   p5->set<cts("uSunDir")>({0, 0, 1});
   p5->set<cts("uEye")>(mCamera.eye());
   auto p6 = gRenderer.get<cts("animationObject")>();
   p6->set<cts("uViewProj")>(mCamera.viewProj());
   p6->set<cts("uRightStabilized")>(glm::normalize(glm::cross({0, 0, 1}, mCamera.back())));
   auto p7 = gRenderer.get<cts("texturedIlluminated")>();
   p7->set<cts("uViewProj")>(mCamera.viewProj());
   p7->set<cts("uSunDir")>({0, 0, 1});
   p7->set<cts("uEye")>(mCamera.eye());
}

void CApp::onKeyPressed(const sf::Event::KeyEvent& keyEvent)
{
   switch (keyEvent.code)
   {
      case sf::Keyboard::Num1:
         mContext.mDrawDebugFrames = !mContext.mDrawDebugFrames;
         Log::msg("debug frames turned ", Log::SOnOff(mContext.mDrawDebugFrames));
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
   gl(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   mTerrain.draw(mContext);
   mSky.draw(mContext);

   gl(glEnable, GL_BLEND);
   for (auto&& ao : mAnimationObjects)
      ao.draw(mContext);
   gl(glDisable, GL_BLEND);

   mAircraft.draw(mContext);
   mSphere.draw(mContext);
}
