/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Engine.hpp"
#include "Log.hpp"
#include "Program.hpp"

CEngine::CEngine()
   : mWindow(sf::VideoMode(800, 600), APPNAME, sf::Style::Titlebar | sf::Style::Close)
{
   sf::ContextSettings settings = mWindow.getSettings();
   Log::msg("openGL ", settings.majorVersion, '.', settings.minorVersion,  " version loaded");
}

void CEngine::run()
{
   static const GLfloat vertexBufferData[] = {
      -0.5f,-0.5f, 0.0f,
       0.5f,-0.5f, 0.0f,
       0.5f, 0.5f, 0.0f,
      -0.5f, 0.5f, 0.0f,
   };
   static const GLubyte indices[] = {0, 1, 3, 2};

   TProgramPtr p = CProgram::create(CShader::createVertexShader(R"(\
attribute vec3 vertex;
void main()
{
   gl_Position.xyz = vertex;
   gl_Position.w = 1.0;
}
)"),
                                 CShader::createFragmentShader(R"(\
void main()
{
   gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
)"));

   GLuint vertexBuffer;
   gl(glGenBuffers, 1, &vertexBuffer);
   gl(glBindBuffer, GL_ARRAY_BUFFER, vertexBuffer);
   gl(glBufferData, GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

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
      gl(glBindBuffer, GL_ARRAY_BUFFER, vertexBuffer);
      gl(glVertexAttribPointer,
         0, // ?
         3, // size
         GL_FLOAT, // type
         GL_FALSE, // not normalized?
         0, // stride
         (void*)0 // offset
         );

      gl(glDrawElements, GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);
      gl(glDisableVertexAttribArray, 0);
      // end the current frame (internally swaps the front and back buffers)
      mWindow.display();
   }
}
