/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMS_HPP
#define ENGINE_PROGRAMS_HPP

#include "Program.hpp"
#include "BufferObjectProgramInput.hpp"
#include "UniformProgramInput.hpp"
#include "Texture.hpp"

using tPosAttrib       = TAttrib<cts("aPos"),   glm::tvec3, float, float, 1>;
using tPosAttribBuffer = TBufferObject<tPosAttrib>;
using tModelUniform    = TUniform<cts("uModel"), glm::tmat4x4, float>;
using tViewProjUniform = TUniform<cts("uViewProj"), glm::tmat4x4, float>;
using tColorUniform    = TUniform<cts("uColor"), glm::tvec3, float>;
using tPosUniform      = TUniform<cts("uPos"), glm::tvec3, float>;
using tSizeUniform     = TUniform<cts("uSize"), glm::tvec2, float>;
using tRightUniform    = TUniform<cts("uRight"), glm::tvec3, float>;
using tUpUniform       = TUniform<cts("uUp"), glm::tvec3, float>;

using tUVAttrib        = TAttrib<cts("aUV"), glm::tvec2, float>;
using tPosUVAttrib     = TAttribPackTraits<tPosAttrib, tUVAttrib>;
using tPosUVAttribBuffer = TBufferObject<tPosUVAttrib>;

inline auto make_program(cts("coloredPolygon"))
{
   return std::make_unique<TProgram<TBufferObjectProgramInput<tPosAttrib>,
                                    TUniformProgramInput<tag::vertex, tModelUniform>,
                                    TUniformProgramInput<tag::vertex, tViewProjUniform>,
                                    TUniformProgramInput<tag::fragment, tColorUniform>>
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
                                    TUniformProgramInput<tag::vertex, tModelUniform>,
                                    TUniformProgramInput<tag::vertex, tViewProjUniform>,
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
                                    TUniformProgramInput<tag::vertex, tPosUniform>,
                                    TUniformProgramInput<tag::vertex, tSizeUniform>,
                                    TUniformProgramInput<tag::vertex, tRightUniform>,
                                    TUniformProgramInput<tag::vertex, tUpUniform>,
                                    TUniformProgramInput<tag::vertex, tViewProjUniform>,
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
using tRenderer = TRenderer<cts("coloredPolygon"), cts("texturedPolygon"), cts("billboard")>;
extern tRenderer gRenderer;

#endif // ENGINE_PROGRAMS_HPP
