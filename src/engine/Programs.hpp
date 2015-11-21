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

inline auto make_program(cts("texturedBillboard"))
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
   gl_Position = uViewProj*vec4(uPos + aPos.x*uSize.x*uRight + aPos.y*uSize.y*uUp, 1.0);
   vUV = aUV;
})",
                             R"(\
varying vec2 vUV;
void main()
{
   gl_FragColor = texture2D(uTexture, vUV);
})");
}

inline auto make_program(cts("healthbar"))
{
   return std::make_unique<TProgram<TBufferObjectProgramInput<TAttrib<cts("aPos"), glm::tvec3, float, float, 1>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uViewProj"), glm::tmat4x4, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uPos"), glm::tvec3, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uExternalPos"), glm::tvec3, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uSize"), glm::tvec2, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uRight"), glm::tvec3, float>>,
                                    TUniformProgramInput<tag::vertex, TUniform<cts("uUp"), glm::tvec3, float>>,
                                    TUniformProgramInput<tag::fragment, TUniform<cts("uValue"), glm::tvec1, float>>>
                           >(R"(\
varying vec2 vPos;
float yCoef = 1.3; // use 30% of uSize.y as offset
float xCoef = (yCoef - 1.0)*uSize.y/uSize.x + 1.0;
void main()
{
   gl_Position = uViewProj*vec4(uPos+uExternalPos + aPos.x*uSize.x*uRight + aPos.y*uSize.y*uUp, 1.0);
   vPos = vec2(xCoef*aPos.x + 0.5,
               yCoef*aPos.y + 0.5);
})",
                             R"(\
varying vec2 vPos;
void main()
{
   vec2 stepRes = step(vec2(0.0, 0.0), vPos) - step(vec2(uValue, 1.0), vPos);
   float colorWeight = stepRes.x * stepRes.y;
   gl_FragColor = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), colorWeight);
})");
}

#include "Renderer.hpp"
using tRenderer = TRenderer<cts("colored"),
                            cts("texturedPolygon"),
                            cts("texturedBillboard"),
                            cts("healthbar")>;
extern tRenderer gRenderer;

#endif // ENGINE_PROGRAMS_HPP
