/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Engine.hpp"
#include "Log.hpp"
#include "Program.hpp"
#include "BufferObject.hpp"

CEngine::CEngine()
   : mWindow(sf::VideoMode(800, 600), APPNAME, sf::Style::Titlebar | sf::Style::Close)
{
   sf::ContextSettings settings = mWindow.getSettings();
   Log::msg("openGL ", settings.majorVersion, '.', settings.minorVersion,  " version loaded");
}

void CEngine::run()
{
   using tPosition = TAttributeData<GL_FLOAT, 3>;
   using tColor    = TAttributeData<GL_FLOAT, 3>;
   using tAttributePack = TAttributeDataPack<tPosition, tColor>;

   static const tAttributePack vertexBufferData[] = {
      { {-0.5f,-0.5f, 0.0f}, {1.f, 1.f, 0.f} },
      { { 0.5f,-0.5f, 0.0f}, {0.f, 1.f, 0.f} },
      { { 0.5f, 0.5f, 0.0f}, {0.f, 1.f, 1.f} },
      { {-0.5f, 0.5f, 0.0f}, {1.f, 1.f, 0.f} }
   };

   TBufferObject<tAttributePack> buf;
   buf.bindData(vertexBufferData, 4);

   static const GLubyte indices[] = {0, 1, 3, 2};

   TProgramPtr p = CProgram::create(CShader::createVertexShader(R"(\
attribute vec4 aVertex;
attribute vec3 aColor;
varying vec3 vColor;
void main()
{
   gl_Position = aVertex;
   vColor = aColor;
}
)"),
                                    CShader::createFragmentShader(R"(\
varying vec3 vColor;
void main()
{
   gl_FragColor.xyz = vColor;
   gl_FragColor.w = 1.0;
}
)"));

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

      // clear the buffers
      gl(glClear, GL_COLOR_BUFFER_BIT);
      p->bind();
      gl(glEnableVertexAttribArray, 0);
      gl(glEnableVertexAttribArray, 1);

      buf.bind({0, 1});

      gl(glDrawElements, GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);
      gl(glDisableVertexAttribArray, 0);
      gl(glDisableVertexAttribArray, 1);
      // end the current frame (internally swaps the front and back buffers)
      mWindow.display();
   }
}
