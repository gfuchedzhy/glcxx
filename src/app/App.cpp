/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "App.hpp"
#include "Programs.hpp"
#include "Texture.hpp"
#include <random>

CApp::CApp()
   : CEngine(800, 600)
   , mSphere({1, 1, 0})
   , mAnimationObjects({32, 32})
{
   mAircraft.pos({0, 0, 1.5e3});

   mSphere.scale({5, 5, 5});
   mSphere.pos(mAircraft.pos() + glm::vec3{0, 20, 0});

   mCamera.perspective(35, mAspect, 0.1, 2e4);
   mCamera.eyeDistance(150);
   mCamera.pitch(20);

   auto cloudTexture = std::make_shared<CTexture>("res/cloud.dds");
   float angle = 0;
   const float bbRadius = 3e3;

   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_real_distribution<> distr1(0.8f, 1.5f);
   std::uniform_real_distribution<> distr2(-0.1f, 0.f);
   for (auto&& c : mClouds)
   {
      c.texture(cloudTexture);
      c.size(glm::vec2(500*distr1(gen), 350*distr1(gen)));
      const float jitteredAngle = distr1(gen)*angle;
      c.pos(glm::vec3(bbRadius*sin(jitteredAngle), bbRadius*cos(jitteredAngle), 1e3 + bbRadius*distr2(gen)));
      angle += 2*M_PI/mClouds.size();
   }

   auto aoTexture = std::make_shared<CTexture>("res/star-sprite.dds");
   for (auto&& ao : mAnimationObjects)
   {
      ao.texture(aoTexture);
      ao.size({5, 5});
      ao.pos({0, 5*distr1(gen), 0});
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

/// @brief animates value returned by get in given range on given keypresses,
/// optionally dumping to middle of the range when no keys pressed
template<typename Get, typename Set>
inline void animate(Get get, Set set,
                    float timeDelta, float speed,
                    sf::Keyboard::Key minKey, sf::Keyboard::Key maxKey,
                    float minVal, float maxVal, bool dump = false)
{
   if (sf::Keyboard::isKeyPressed(minKey))
      set(std::max(minVal, get() - timeDelta*speed));
   else if (sf::Keyboard::isKeyPressed(maxKey))
      set(std::min(maxVal, get() + timeDelta*speed));
   else if (dump)
      dumpAnimation(get, set, timeDelta, 0.5f*(minVal+maxVal));
}

void CApp::update(float timeDelta)
{
   mAircraft.update(timeDelta);
   for (auto&& ao : mAnimationObjects)
      ao.update(timeDelta);

   mCamera.lookAt(mAircraft.pos());

   static const float angularSpeed = 90;
   float angle = mSphere.roll() + timeDelta*angularSpeed;
   angle = angle - (int(angle)/360)*360;
   mSphere.roll(angle);

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
              timeDelta, 70.f, sf::Keyboard::Add, sf::Keyboard::Subtract, 30.f, 500.f);

      dumpAnimation([this]{return mAircraft.pitch();},
                    [this](float val){mAircraft.pitch(val);},
                    timeDelta, 0);
      dumpAnimation([this]{return mAircraft.roll();},
                    [this](float val){mAircraft.roll(val);},
                    timeDelta, 0);
   }
   else
   {
      animate([this]{return mAircraft.pitch();},
              [this](float val){mAircraft.pitch(val);},
              timeDelta, 70.f, sf::Keyboard::Up, sf::Keyboard::Down, -60.f, 60.f, true);

      animate([this]{return mAircraft.roll();},
              [this](float val){mAircraft.roll(val);},
              timeDelta, 70.f, sf::Keyboard::Left, sf::Keyboard::Right, -60.f, 60.f, true);
   }

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
      default:
         break;
   }
}

void CApp::draw() const
{
   mTerrain.draw(mContext);
   gl(glDisable, GL_DEPTH_TEST);
   mSky.draw(mContext);
   gl(glEnable, GL_DEPTH_TEST);

   gl(glEnable, GL_BLEND);
   gl(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   for (auto&& c : mClouds)
      c.draw(mContext);

   for (auto&& ao : mAnimationObjects)
      ao.draw(mContext);

   gl(glDisable, GL_BLEND);

   mAircraft.draw(mContext);
   mSphere.draw(mContext);
}
