/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Aircraft.hpp"
#include "Box.hpp"
#include <glm/gtx/transform.hpp>

CAircraft::CAircraft()
{
   using namespace std;
   using namespace glm;

   auto make_box = [] (vec3 color, vec3 size) {
      auto p = make_shared<CBox>(color);
      p->scale(size);
      return p;
   };

   // cubed geometry representing airplain, kind of :)
   const vec3 bodySize(3.f, 22.f, 3.f);
   const size_t bodyPartNum = 4;
   const vec3 bodyPartSize(3.f, bodySize.y/bodyPartNum, 3.f);
   const vec3 bodyColor(1.f, 1.f, 0.f);

   auto body = make_shared<CComplexRenderable>();
   for (size_t i = 0; i < bodyPartNum; ++i)
   {
      auto p = make_box(bodyColor, bodyPartSize);
      p->pos(vec3(0.f, -(i + 0.5f)*bodyPartSize.y, 0.f));
      body->append({p});
   }

   const vec3 tailColor(1.f, 1.f, 1.f);
   auto hTail = make_box(tailColor, {9.f, 5.f, 0.2f});
   auto vTail = make_box(tailColor, {0.2f, 5.f, 5.f});
   hTail->pos(vec3(0.f, 2.5f, 0.f));
   vTail->pos(vec3(0.f, 2.5f, 2.5f));
   auto tail = make_shared<CComplexRenderable>(tIniList{hTail, vTail});
   tail->pos(vec3(0.f, -bodySize.y, 0.f));

   const vec3 wingColor(0.f, 1.f, 0.f);
   const vec3 wingSize(12.f, 4.f, 0.2f);
   auto lWing = make_box(wingColor, wingSize);
   auto rWing = make_box(wingColor, wingSize);
   lWing->yaw( 20.f);
   rWing->yaw(-20.f);
   lWing->pos(vec3(-wingSize.x/2.f, 0.f, 0.f));
   rWing->pos(vec3( wingSize.x/2.f, 0.f, 0.f));
   auto wings = make_shared<CComplexRenderable>(tIniList{lWing, rWing});
   wings->pos(vec3(0.f, -bodySize.y/3.f, 0.f));

   const vec3 propColor(0.f, 1.f, 1.f);
   const vec3 propSize(1.f, 0.2f, 12.f);
   const size_t propNum = 3;
   auto prop = make_shared<CComplexRenderable>();
   for (size_t i = 0u; i < propNum; ++i)
   {
      auto blade = make_box(propColor, propSize);
      blade->roll(i*180.f/propNum);
      prop->append({blade});
   }
   auto shaft = make_box(propColor, {0.3f, 0.8f, 0.3f});
   shaft->pos(vec3(0.f, -0.4f, 0.f));
   prop->append({shaft});
   prop->pos(vec3(0.f, 0.4f, 0.f));
   auto wholeBody = make_shared<CComplexRenderable>(tIniList{body, tail, wings, prop});
   wholeBody->pos(vec3(0.f, bodySize.y*0.2f, -bodySize.z/2));
   auto cabin = make_box({1.f, 0.f, 0.f}, {1.5f, 3.f, 2.f});
   append(tIniList{wholeBody, cabin});
   mProp = prop.get();
}

void CAircraft::update(float timeDelta)
{
   static const float angularSpeed = 3e3f;
   float angle = mProp->roll() + timeDelta*angularSpeed;
   angle = angle - (int(angle)/360)*360;
   mProp->roll(angle);
}
