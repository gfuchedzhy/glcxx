/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMS_HPP
#define ENGINE_PROGRAMS_HPP

#include "Program.hpp"
#include "Texture.hpp"
#include <functional>

template<typename TName>
const char* programSrc();

#define inp_by_name(name) decltype(progInputDef(cts(name){}))

auto progInputDef(cts("regular")) -> std::tuple<
   TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
   TProgramInput<cts("uModel"), TUniform<glm::tmat4x4, float>>,
   TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4, float>>>;

auto progInputDef(cts("regular-col")) -> ct::tuple_append<inp_by_name("regular"),
   TProgramInput<cts("uColor"), TUniform<glm::tvec3, float>, tag::fragment>>;

auto progInputDef(cts("regular-tex")) -> ct::tuple_append<inp_by_name("regular"),
   TProgramInput<cts("aUV"), TAttrib<glm::tvec2, float>>,
   TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>>;

auto progInputDef(cts("shaded")) -> std::tuple<
   TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
   TProgramInput<cts("aNorm"), TAttrib<glm::tvec3, float>>,
   TProgramInput<cts("uModel"), TUniform<glm::tmat4x4, float>>,
   TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4, float>>,
   TProgramInput<cts("uAmbient"), TUniform<glm::tvec3, float>, tag::fragment>,
   TProgramInput<cts("uDiffuse"), TUniform<glm::tvec3, float>, tag::fragment>,
   TProgramInput<cts("uSpecular"), TUniform<glm::tvec3, float>, tag::fragment>,
   TProgramInput<cts("uShininess"), TUniform<glm::tvec1, float>, tag::fragment>,
   TProgramInput<cts("uSunDir"), TUniform<glm::tvec3, float>, tag::all>,
   TProgramInput<cts("uEye"), TUniform<glm::tvec3, float>>>;

auto progInputDef(cts("shaded-col")) -> ct::tuple_append<inp_by_name("shaded"),
   TProgramInput<cts("uColor"), TUniform<glm::tvec3, float>, tag::fragment>>;

auto progInputDef(cts("shaded-tex")) -> ct::tuple_append<inp_by_name("shaded"),
   TProgramInput<cts("aUV"), TAttrib<glm::tvec2, float>>,
   TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>>;

auto progInputDef(cts("shaded-tex-nmap")) -> ct::tuple_append<inp_by_name("shaded-tex"),
   TProgramInput<cts("aTan"), TAttrib<glm::tvec3, float>>,
   TProgramInput<cts("uNormalMap"), TTextureProgramInput<1>, tag::fragment>>;

auto progInputDef(cts("billboard")) -> std::tuple<
   TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
   TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4, float>>,
   TProgramInput<cts("uPos"), TUniform<glm::tvec3, float>>,
   TProgramInput<cts("uExternalPos"), TUniform<glm::tvec3, float>>,
   TProgramInput<cts("uSize"), TUniform<glm::tvec2, float>>,
   TProgramInput<cts("uRight"), TUniform<glm::tvec3, float>>,
   TProgramInput<cts("uUp"), TUniform<glm::tvec3, float>>>;

auto progInputDef(cts("billboard-tex")) -> ct::tuple_append<inp_by_name("billboard"),
   TProgramInput<cts("aUV"), TAttrib<glm::tvec2, float>>,
   TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>>;

auto progInputDef(cts("billboard-tex-sprite")) -> ct::tuple_append<inp_by_name("billboard-tex"),
   TProgramInput<cts("uAtlasSize"), TUniform<glm::tvec2, int>>,
   TProgramInput<cts("uAtlasPos"), TUniform<glm::tvec2, int>>>;

auto progInputDef(cts("billboard-hb")) -> ct::tuple_append<inp_by_name("billboard"),
   TProgramInput<cts("uValue"), TUniform<glm::tvec1, float>, tag::fragment>>;

#include "Renderer.hpp"

using tRenderer = TRenderer<cts("regular-col"),
                            cts("regular-tex"),
                            cts("shaded-col"),
                            cts("shaded-tex"),
                            cts("shaded-tex-nmap"),
                            cts("billboard-tex"),
                            cts("billboard-tex-sprite"),
                            cts("billboard-hb")>;
extern tRenderer gRenderer;

#endif // ENGINE_PROGRAMS_HPP
