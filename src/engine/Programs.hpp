/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMS_HPP
#define ENGINE_PROGRAMS_HPP

#include "Program.hpp"
#include "BufferObjectProgramInput.hpp"
#include "UniformProgramInput.hpp"
#include "Texture.hpp"

using tPosUVAttrib  = TAttribPackTraits<TAttrib<cts("aPos"), glm::tvec3, float, float, 1>,
                                        TAttrib<cts("aUV"), glm::tvec2, float>>;

inline auto make_program(cts("colored"))
{
   return std::make_unique<TProgram<TBufferObjectProgramInput<TAttrib<cts("aPos"), glm::tvec3, float, float, 1>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uModel"), glm::tmat4x4, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uViewProj"), glm::tmat4x4, float>>,
                                    TUniformProgramInput<tag::fragment, TUniform<cts("uColor"), glm::tvec3, float>>>
                           >(R"(\
void main()
{
   gl_Position = uViewProj*uModel*aPos;
})",
                             R"(\
void main()
{
   gl_FragColor.xyz = uColor;
   gl_FragColor.w = 1.0;
})");
}

inline auto make_program(cts("texturedPolygon"))
{
   return std::make_unique<TProgram<TBufferObjectProgramInput<tPosUVAttrib>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uModel"), glm::tmat4x4, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uViewProj"), glm::tmat4x4, float>>,
                                    TTextureProgramInput<cts("uTexture")>>
                           >(R"(\
varying vec2 vUV;
void main()
{
   gl_Position = uViewProj*uModel*aPos;
   vUV = aUV;
})",
                             R"(\
varying vec2 vUV;
void main()
{
   gl_FragColor = texture2D(uTexture, vUV);
})");
}

inline auto make_program(cts("billboard"))
{
   return std::make_unique<TProgram<TBufferObjectProgramInput<tPosUVAttrib>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uViewProj"), glm::tmat4x4, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uPos"), glm::tvec3, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uSize"), glm::tvec2, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uRight"), glm::tvec3, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uUp"), glm::tvec3, float>>,
                                    TTextureProgramInput<cts("uTexture")>>
                           >(R"(\
varying vec2 vUV;
void main()
{
   vec4 pos;
   pos.xyz = uPos + aPos.x*uSize.x*uRight + aPos.y*uSize.y*uUp;
   pos.w = 1.0;
   gl_Position = uViewProj*pos;
   vUV = aUV;
})",
                             R"(\
varying vec2 vUV;
void main()
{
   gl_FragColor = texture2D(uTexture, vUV);
})");
}

#include "Renderer.hpp"
using tRenderer = TRenderer<cts("colored"), cts("texturedPolygon"), cts("billboard")>;
extern tRenderer gRenderer;

#endif // ENGINE_PROGRAMS_HPP
