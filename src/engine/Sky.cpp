/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Sky.hpp"
#include "Texture.hpp"
#include "Programs.hpp"
#include <random>

CSky::CSky()
{
   const float size = 1e4;
   auto tex = std::make_shared<CTexture>("res/sky.dds");
   std::array<std::shared_ptr<CTexturedRect>, 5> sky;
   for (auto&& x : sky)
   {
      x = std::make_shared<CTexturedRect>();
      x->texture(tex);
      x->scale(glm::vec3(size, size, size));
      append({x});
   }
   sky[0]->pitch(90);
   sky[0]->pos(glm::vec3(0, size/2, 0));
   sky[1]->pitch(180);
   sky[1]->pos(glm::vec3(0, 0, size/2));
   sky[2]->pitch(90);
   sky[2]->roll(90);
   sky[2]->pos(glm::vec3(-size/2, 0, 0));
   sky[3]->pitch(90);
   sky[3]->roll(-90);
   sky[3]->pos(glm::vec3(size/2, 0, 0));
   sky[4]->pitch(-90);
   sky[4]->pos(glm::vec3(0, -size/2, 0));

   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_real_distribution<> distr1(0.8f, 1.5f);
   std::uniform_real_distribution<> distr2(-0.1f, 0.f);
   float angle = 0;
   const float bbRadius = 3e3;
   auto cloudTexture = std::make_shared<CTexture>("res/cloud.dds");
   for (auto&& c : mClouds)
   {
      c.texture(cloudTexture);
      c.size(glm::vec2(500*distr1(gen), 350*distr1(gen)));
      const float jitteredAngle = distr1(gen)*angle;
      c.pos(glm::vec3(bbRadius*sin(jitteredAngle), bbRadius*cos(jitteredAngle), 1e3 + // clouds around 1km height
                      bbRadius*distr2(gen)));
      angle += 2*M_PI/mClouds.size();
   }

   pos(glm::vec3(0, 0, size/2));
}

void CSky::draw(const SContext& context) const
{
   gl(glDisable, GL_DEPTH_TEST);
   CComplexRenderable::draw(context);
   gl(glEnable, GL_DEPTH_TEST);

   gl(glEnable, GL_BLEND);
   // @todo this is a hack, should rethink model of complex objects
   auto p = gRenderer.getAndSelect<cts("texturedBillboard")>();
   p->set<cts("uExternalPos")>({pos().x, pos().y, 0});
   for (auto&& c : mClouds)
   {
      c.draw(context);
   }
   p->set<cts("uExternalPos")>({0, 0, 0});
   gl(glDisable, GL_BLEND);
}
