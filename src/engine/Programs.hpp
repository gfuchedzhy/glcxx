/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMS_HPP
#define ENGINE_PROGRAMS_HPP

#include "Program.hpp"
#include "BufferObjectProgramInput.hpp"
#include "UniformProgramInput.hpp"

using tPosAttrib       = TAttrib<cts("aPos"),   glm::tvec3, float, float, 1>;
using tPosAttribBuffer = TBufferObject<tPosAttrib>;
using tModelUniform    = TUniform<cts("uModel"), glm::tmat4x4, float>;
using tColorUniform    = TUniform<cts("uColor"), glm::tvec3, float>;

inline auto make_program(cts("coloredPolygon"))
{
   return std::make_unique<TProgram<TBufferObjectProgramInput<tPosAttrib>,
                                    TUniformProgramInput<tag::vertex, tModelUniform>,
                                    TUniformProgramInput<tag::fragment, tColorUniform>>
                           >(R"(\
void main()
{
   gl_Position = uModel*aPos;
})",
                             R"(\
void main()
{
   gl_FragColor.xyz = uColor;
   gl_FragColor.w = 1.0;
})");
}

#include "Renderer.hpp"
using tRenderer = TRenderer<cts("coloredPolygon")>;
extern tRenderer gRenderer;

#endif // ENGINE_PROGRAMS_HPP
