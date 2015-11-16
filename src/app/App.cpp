/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "App.hpp"
#include "Programs.hpp"
#include "Texture.hpp"

CApp::CApp()
   : CEngine(800, 600)
{
   mCamera.perspective(35.f, mAspect, 0.1f, 3e2f);
   mCamera.eyeDistance(50.f);
   mCamera.pitch(10.f);

   mGround.texture(std::make_shared<CTexture>("ground.dds"));
   mGround.scale(glm::vec3(200, 200, 200));
   mGround.pos(glm::vec3(0, 0, -0.25f*200));

   auto cloudTexture = std::make_shared<CTexture>("cloud.dds");
   float angle = 0;
   const float bbRadius = 100.f;
   for (auto&& b : mBillboards)
   {
      b.texture(cloudTexture);
      b.size(glm::vec2(30, 15));
      b.pos(glm::vec3(bbRadius*sin(angle), bbRadius*cos(angle), 0));
      angle += 2*M_PI/mBillboards.size();
   }
}

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
      if (abs(get() - middle) > 0.001f)
         set(middle + (get() - middle) * std::max(0.f, 1.f - timeDelta/0.2f));
      else
         set(middle);
   }
}

void CApp::update(float timeDelta)
{
   mAircraft.update(timeDelta);

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

   auto p = gRenderer.get<cts("coloredPolygon")>();
   p->set<cts("uViewProj")>(mCamera.viewProj());
   auto p2 = gRenderer.get<cts("texturedPolygon")>();
   p2->set<cts("uViewProj")>(mCamera.viewProj());
   auto p3 = gRenderer.get<cts("billboard")>();
   p3->set<cts("uViewProj")>(mCamera.viewProj());
   p3->set<cts("uUp")>(mCamera.up());
   p3->set<cts("uRight")>(mCamera.right());

}

void CApp::draw() const
{
   gl(glDisable, GL_DEPTH_TEST);
   mSky.draw();
   mGround.draw();
   gl(glEnable, GL_DEPTH_TEST);

   for (auto&& b : mBillboards)
      b.draw();

   mAircraft.draw();
}
