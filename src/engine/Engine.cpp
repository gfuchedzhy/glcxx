/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Engine.hpp"
#include "Log.hpp"
#include "Program.hpp"
#include "BufferObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

CEngine::CEngine()
   : mWindow(sf::VideoMode(800, 600), APPNAME, sf::Style::Titlebar | sf::Style::Close,
             sf::ContextSettings(24, 0, 0, 3, 0))
{
   sf::ContextSettings settings = mWindow.getSettings();
   Log::msg("openGL ", settings.majorVersion, '.', settings.minorVersion,  " version loaded");
}

void CEngine::run()
{
   using tAttributePack = TAttributeDataPack<TAttributeData<GL_FLOAT, 3, 1, cts("aPosition")>,
                                             TAttributeData<GL_FLOAT, 3, 0, cts("aColor")>>;

   static const tAttributePack vertexBufferData[] = {
      { {-0.5f,-0.5f, 0.5f}, {0.f, 1.f, 1.f} },
      { { 0.5f,-0.5f, 0.5f}, {0.f, 1.f, 0.f} },
      { { 0.5f, 0.5f, 0.5f}, {1.f, 1.f, 0.f} },
      { {-0.5f, 0.5f, 0.5f}, {1.f, 0.f, 0.f} },
      { {-0.5f,-0.5f,-0.5f}, {1.f, 1.f, 0.f} },
      { { 0.5f,-0.5f,-0.5f}, {0.f, 1.f, 0.f} },
      { { 0.5f, 0.5f,-0.5f}, {0.f, 1.f, 1.f} },
      { {-0.5f, 0.5f,-0.5f}, {0.f, 0.f, 1.f} }
   };

   auto prog = std::make_shared<TProgram<TBufferObject<tAttributePack>>>(
      R"(\
uniform mat4 uModel;
varying vec3 vColor;
void main()
{
   gl_Position = uModel*aPosition;
   vColor = aColor;
})",
      R"(\
varying vec3 vColor;
void main()
{
   gl_FragColor.xyz = vColor;
   gl_FragColor.w = 1.0;
})");

   auto buf = std::make_shared<TBufferObject<tAttributePack>>();
   buf->upload(vertexBufferData, 8);

   prog->set<cts("aPosition,aColor")>(buf);
   CProgramObject::select(prog);

   static const GLubyte indices[] = {0, 1, 3, 2, 7, 6, 4, 5, 0, 1,
                                     1, 5, 2, 6,
                                     6, 3, 3, 7, 0, 4};

   const float aspect = mWindow.getSize().x / float(mWindow.getSize().y);
   const glm::mat4 proj = glm::perspective(45.0f, aspect, 0.1f, 100.f);
   const glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -3.f));
   const glm::mat4 rotateX = glm::rotate(glm::mat4(), 0.5f, glm::vec3(1.f, 0.f, 0.f));
   const glm::mat4 projTransRotX = proj*translate*rotateX;

   gl(glEnable, GL_DEPTH_TEST);
   gl(glEnable, GL_CULL_FACE);
   float angle = 0.f;
   while (true)
   {
      // handle events
      sf::Event event;
      while (mWindow.pollEvent(event))
      {
         switch(event.type)
         {
            case sf::Event::Closed:
               Log::msg("close event occured");
               return;
            default:
               break;
         }
      }

      angle += 0.0001f;
      angle = angle > 360.f ? angle - 360 : angle;
      // clear the buffers
      gl(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glm::mat4 result = projTransRotX*glm::rotate(glm::mat4(), angle, glm::vec3(0.f, 1.f, 0.f));
      gl(glUniformMatrix4fv, 0, 1, GL_FALSE, &result[0][0]);
      gl(glDrawElements, GL_TRIANGLE_STRIP, sizeof(indices), GL_UNSIGNED_BYTE, indices);
      // end the current frame (internally swaps the front and back buffers)
      mWindow.display();
   }
}
