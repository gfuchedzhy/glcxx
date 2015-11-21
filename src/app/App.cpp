/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "App.hpp"
#include "Programs.hpp"
#include "Texture.hpp"
#include <random>

CApp::CApp()
   : CEngine(800, 600)
   , mSphere({1.f, 0.f, 1.f})
{
   mSphere.scale({5, 5, 5});
   mSphere.pos({0, 20, 0});

   mCamera.perspective(35.f, mAspect, 0.1f, 3e3f);
   mCamera.eyeDistance(50.f);
   mCamera.pitch(10.f);

   mGround.texture(std::make_shared<CTexture>("res/ground.dds"));
   const float scale = 2e3f;
   mGround.scale(glm::vec3(scale, scale, scale));
   mGround.pos(glm::vec3(0, 0, -0.1f*scale));

   auto cloudTexture = std::make_shared<CTexture>("res/cloud.dds");
   float angle = 0;
   const float bbRadius = 800.f;

   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_real_distribution<> distr1(0.8f, 1.5f);
   std::uniform_real_distribution<> distr2(-0.4f, 0.f);
   for (auto&& c : mClouds)
   {
      c.texture(cloudTexture);
      c.size(glm::vec2(250*distr1(gen), 150*distr1(gen)));
      const float jitteredAngle = distr1(gen)*angle;
      c.pos(glm::vec3(bbRadius*sin(jitteredAngle), bbRadius*cos(jitteredAngle), bbRadius*distr2(gen)));
      angle += 2*M_PI/mClouds.size();
   }
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
   {
      const float middle = 0.5f*(minVal+maxVal);
      if (abs(get() - middle) > 0.001f) /// @todo there should be a better way
         set(middle + (get() - middle) * std::max(0.f, 1.f - timeDelta/0.2f));
      else
         set(middle);
   }
}

void CApp::update(float timeDelta)
{
   mAircraft.update(timeDelta);

   static const float angularSpeed = 90;
   float angle = mSphere.roll() + timeDelta*angularSpeed;
   angle = angle - (int(angle)/360)*360;
   mSphere.roll(angle);

   animate([this]{return mCamera.orientation();},
           [this](float val){mCamera.orientation(val);},
           timeDelta, 70.f, sf::Keyboard::Left, sf::Keyboard::Right, -180.f, 180.f);

   animate([this]{return mCamera.pitch();},
           [this](float val){mCamera.pitch(val);},
           timeDelta, 70.f, sf::Keyboard::Down, sf::Keyboard::Up, -30.f, 90.f);

   animate([this]{return mCamera.eyeDistance();},
           [this](float val){mCamera.eyeDistance(val);},
           timeDelta, 70.f, sf::Keyboard::Add, sf::Keyboard::Subtract, 30.f, 100.f);

   animate([this]{return mAircraft.pitch();},
           [this](float val){mAircraft.pitch(val);},
           timeDelta, 70.f, sf::Keyboard::W, sf::Keyboard::S, -60.f, 60.f, true);

   animate([this]{return mAircraft.roll();},
           [this](float val){mAircraft.roll(val);},
           timeDelta, 70.f, sf::Keyboard::A, sf::Keyboard::D, -60.f, 60.f, true);

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
   }
}

void CApp::draw() const
{
   gl(glDisable, GL_DEPTH_TEST);
   mSky.draw(mContext);
   mGround.draw(mContext);
   gl(glEnable, GL_DEPTH_TEST);

   gl(glEnable, GL_BLEND);
   gl(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   for (auto&& c : mClouds)
      c.draw(mContext);

   gl(glDisable, GL_BLEND);

   mAircraft.draw(mContext);
   mSphere.draw(mContext);
}
