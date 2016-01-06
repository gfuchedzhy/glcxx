/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
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
   TProgramInput<cts("uModel"), TUniform<glm::tmat4x4>>,
   TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4>>>;

auto progInputDef(cts("regular-col")) -> ct::tuple_append<inp_by_name("regular"),
   TProgramInput<cts("uColor"), TUniform<glm::tvec3>, tag::fragment>>;

auto progInputDef(cts("regular-tex")) -> ct::tuple_append<inp_by_name("regular"),
   TProgramInput<cts("aUV"), TAttrib<glm::tvec2>>,
   TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>>;

auto progInputDef(cts("shaded")) -> std::tuple<
   TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
   TProgramInput<cts("aNorm"), TAttrib<glm::tvec3>>,
   TProgramInput<cts("uModel"), TUniform<glm::tmat4x4>>,
   TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4>>,
   TProgramInput<cts("uAmbient"), TUniform<glm::tvec3>, tag::fragment>,
   TProgramInput<cts("uDiffuse"), TUniform<glm::tvec3>, tag::fragment>,
   TProgramInput<cts("uSpecular"), TUniform<glm::tvec3>, tag::fragment>,
   TProgramInput<cts("uShininess"), TUniform<glm::tvec1>, tag::fragment>,
   TProgramInput<cts("uSunDir"), TUniform<glm::tvec3>, tag::vertfrag>,
   TProgramInput<cts("uEye"), TUniform<glm::tvec3>>>;

auto progInputDef(cts("shaded-col")) -> ct::tuple_append<inp_by_name("shaded"),
   TProgramInput<cts("uColor"), TUniform<glm::tvec3>, tag::fragment>>;

auto progInputDef(cts("shaded-tex")) -> ct::tuple_append<inp_by_name("shaded"),
   TProgramInput<cts("aUV"), TAttrib<glm::tvec2>>,
   TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>>;

auto progInputDef(cts("shaded-tex-nmap")) -> ct::tuple_append<inp_by_name("shaded-tex"),
   TProgramInput<cts("aTan"), TAttrib<glm::tvec3>>,
   TProgramInput<cts("uNormalMap"), TTextureProgramInput<1>, tag::fragment>>;

auto progInputDef(cts("billboard")) -> std::tuple<
   TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
   TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4>>,
   TProgramInput<cts("uPos"), TUniform<glm::tvec3>>,
   TProgramInput<cts("uExternalPos"), TUniform<glm::tvec3>>,
   TProgramInput<cts("uSize"), TUniform<glm::tvec2>>,
   TProgramInput<cts("uRight"), TUniform<glm::tvec3>>,
   TProgramInput<cts("uUp"), TUniform<glm::tvec3>>>;

auto progInputDef(cts("billboard-tex")) -> ct::tuple_append<inp_by_name("billboard"),
   TProgramInput<cts("aUV"), TAttrib<glm::tvec2>>,
   TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>>;

auto progInputDef(cts("billboard-tex-sprite")) -> ct::tuple_append<inp_by_name("billboard-tex"),
   TProgramInput<cts("uAtlasSize"), TUniform<glm::tvec2, int>>,
   TProgramInput<cts("uAtlasPos"), TUniform<glm::tvec2, int>>>;

auto progInputDef(cts("billboard-hb")) -> ct::tuple_append<inp_by_name("billboard"),
   TProgramInput<cts("uValue"), TUniform<glm::tvec1>, tag::fragment>>;

auto progInputDef(cts("particlesys")) -> std::tuple<
   TProgramInput<cts("aPos"), TAttrib<glm::tvec3>>,
   TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4>>,
   TProgramInput<cts("uSize"), TUniform<glm::tvec2>, tag::geometry>,
   TProgramInput<cts("uPerspectiveScale"), TUniform<glm::tvec2>, tag::geometry>>;

auto progInputDef(cts("particlesys-tex")) -> ct::tuple_append<inp_by_name("particlesys"),
   TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>>;

auto progInputDef(cts("particlesys-tex-sprite")) -> ct::tuple_append<inp_by_name("particlesys-tex"),
   // aTime.x is age of particle, aTime.y is age of death
   TProgramInput<cts("aTime"), TAttrib<glm::tvec2>>,
   // for optimization purposes aSpeed.xyz is unit speed directon, aSpeed.w is
   // scalar speed value
   TProgramInput<cts("aSpeed"), TAttrib<glm::tvec4>>,
   TProgramInput<cts("uAtlasSize"), TUniform<glm::tvec2, int, int>, tag::geometry>>;

auto progInputDef(cts("particlesys-tex-sprite-flame")) -> inp_by_name("particlesys-tex-sprite");

#include "Renderer.hpp"

using tRenderer = TRenderer<cts("regular-col"),
                            cts("regular-tex"),
                            cts("shaded-col"),
                            cts("shaded-tex"),
                            cts("shaded-tex-nmap"),
                            cts("billboard-tex"),
                            cts("billboard-tex-sprite"),
                            cts("billboard-hb"),
                            cts("particlesys-tex-sprite-flame")>;
extern tRenderer gRenderer;

#endif // ENGINE_PROGRAMS_HPP
