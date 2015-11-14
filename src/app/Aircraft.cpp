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

   auto make_box = [] (vec3 color, vec3 dimensions) { return static_pointer_cast<IRenderableModel>(make_shared<CBox>(color, dimensions)); };

   // cubed geometry representing airplain, kind of :)
   const float length    = 22.f;
   const float thickness = 3.f;
   const size_t bodyPartNum = 4;
   const size_t bodyPartLen = length/bodyPartNum;
   const vec3 bodyPartDim(thickness, bodyPartLen, thickness);
   const vec3 bodyColor(1.f, 1.f, 0.f);

   auto body = make_shared<CComplexRenderable>();
   for (size_t i = 0; i < bodyPartNum; ++i)
   {
      body->append({make_pair(translate(vec3(0.f, -(i + 0.5f)*bodyPartLen, 0.f)), make_box(bodyColor, bodyPartDim))});
   }

   const vec3 tailColor(1.f, 1.f, 1.f);
   auto tail = make_shared<CComplexRenderable>(std::initializer_list<CComplexRenderable::tObject>
      {
         make_pair(translate(vec3(0.f, 5.f, 0.f)),  make_box(tailColor, {9.f, 5.f, 0.2f})),
         make_pair(translate(vec3(0.f, 5.f, 2.5f)), make_box(tailColor, {0.2f, 5.f, 5.f}))
      });

   vec3 wingColor(0.f, 1.f, 0.f);
   vec3 wingDim(12.f, 4.f, 0.2f);
   auto wings = make_shared<CComplexRenderable>(std::initializer_list<CComplexRenderable::tObject>
      {
         make_pair(rotate(-0.2f, vec3(0.f, 0.f, 1.f)) * translate(vec3( wingDim.x/2, 0.f, 0.f)), make_box(wingColor, wingDim)),
         make_pair(rotate( 0.2f, vec3(0.f, 0.f, 1.f)) * translate(vec3(-wingDim.x/2, 0.f, 0.f)), make_box(wingColor, wingDim))
      });

   vec3 propColor(0.f, 1.f, 1.f);
   vec3 propDim(1.f, 0.2f, 6.f);
   auto prop = make_shared<CComplexRenderable>(std::initializer_list<CComplexRenderable::tObject>
      { make_pair(rotate(0.f,             vec3(0.f, 1.f, 0.f)) * translate(vec3(0.f, 0.f, propDim.z/2)), make_box(propColor, propDim)),
        make_pair(rotate(radians( 120.f), vec3(0.f, 1.f, 0.f)) * translate(vec3(0.f, 0.f, propDim.z/2)), make_box(propColor, propDim)),
        make_pair(rotate(radians(-120.f), vec3(0.f, 1.f, 0.f)) * translate(vec3(0.f, 0.f, propDim.z/2)), make_box(propColor, propDim)),
        make_pair(translate(vec3(0.f, -0.25f, 0.f)), make_box(propColor, {0.3f, 0.5f, 0.3f}))
      });

   auto wholeBody = make_shared<CComplexRenderable>(std::initializer_list<CComplexRenderable::tObject>
      {
         make_pair(mat4(), body),
         make_pair(translate(vec3(0.f, -length, 0.f)), tail),
         make_pair(translate(vec3(0.f, -length/3, 0.f)), wings),
         make_pair(translate(vec3(0.f, 0.5f, 0.f)), prop)
      });

   auto cabin = make_box({1.f, 0.f, 0.f}, {1.5f, 3.f, 2.f});

   append( {make_pair(mat4(), cabin),
            make_pair(translate(vec3(0.f, length*0.2f, -thickness/2)), wholeBody)
            });
}
