/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMS_HPP
#define ENGINE_PROGRAMS_HPP

#include "Program.hpp"
#include "Texture.hpp"
#include "VAOProgramInput.hpp"
#include "UniformProgramInput.hpp"
#include <functional>

template<typename TName>
const char* programSrc();

#define inp_by_name(name) decltype(progInputDef(cts(name){}))

auto progInputDef(cts("regular")) -> std::tuple<
   TNamedAttrib<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
   TUniformProgramInput<cts("uModel"), TUniform<glm::tmat4x4>>,
   TUniformProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4>>>;

auto progInputDef(cts("regular-col")) -> ct::tuple_append<inp_by_name("regular"),
   TUniformProgramInput<cts("uColor"), TUniform<glm::tvec3>, tag::fragment>>;

auto progInputDef(cts("regular-tex")) -> ct::tuple_append<inp_by_name("regular"),
   TNamedAttrib<cts("aUV"), TAttrib<glm::tvec2>>,
   TTextureProgramInput<cts("uTexture")>>;

auto progInputDef(cts("shaded")) -> std::tuple<
   TNamedAttrib<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
   TNamedAttrib<cts("aNorm"), TAttrib<glm::tvec3>>,
   TUniformProgramInput<cts("uModel"), TUniform<glm::tmat4x4>>,
   TUniformProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4>>,
   TUniformProgramInput<cts("uAmbient"), TUniform<glm::tvec3>, tag::fragment>,
   TUniformProgramInput<cts("uDiffuse"), TUniform<glm::tvec3>, tag::fragment>,
   TUniformProgramInput<cts("uSpecular"), TUniform<glm::tvec3>, tag::fragment>,
   TUniformProgramInput<cts("uShininess"), TUniform<glm::tvec1>, tag::fragment>,
   TUniformProgramInput<cts("uSunDir"), TUniform<glm::tvec3>, tag::vertfrag>,
   TUniformProgramInput<cts("uEye"), TUniform<glm::tvec3>, tag::vertex>>;

auto progInputDef(cts("shaded-col")) -> ct::tuple_append<inp_by_name("shaded"),
   TUniformProgramInput<cts("uColor"), TUniform<glm::tvec3>, tag::fragment>>;

auto progInputDef(cts("shaded-tex")) -> ct::tuple_append<inp_by_name("shaded"),
   TNamedAttrib<cts("aUV"), TAttrib<glm::tvec2>>,
   TTextureProgramInput<cts("uTexture")>>;

auto progInputDef(cts("shaded-tex-nmap")) -> ct::tuple_append<inp_by_name("shaded-tex"),
   TNamedAttrib<cts("aTan"), TAttrib<glm::tvec3>>,
   TTextureProgramInput<cts("uNormalMap"), 1>>;

auto progInputDef(cts("billboard")) -> std::tuple<
   TNamedAttrib<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
   TUniformProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4>>,
   TUniformProgramInput<cts("uPos"), TUniform<glm::tvec3>>,
   TUniformProgramInput<cts("uExternalPos"), TUniform<glm::tvec3>>,
   TUniformProgramInput<cts("uSize"), TUniform<glm::tvec2>>,
   TUniformProgramInput<cts("uRight"), TUniform<glm::tvec3>>,
   TUniformProgramInput<cts("uUp"), TUniform<glm::tvec3>>>;

auto progInputDef(cts("billboard-tex")) -> ct::tuple_append<inp_by_name("billboard"),
   TNamedAttrib<cts("aUV"), TAttrib<glm::tvec2>>,
   TTextureProgramInput<cts("uTexture")>>;

auto progInputDef(cts("billboard-tex-sprite")) -> ct::tuple_append<inp_by_name("billboard-tex"),
   TUniformProgramInput<cts("uAtlasSize"), TUniform<glm::tvec2, int>>,
   TUniformProgramInput<cts("uAtlasPos"), TUniform<glm::tvec2, int>>>;

auto progInputDef(cts("billboard-hb")) -> ct::tuple_append<inp_by_name("billboard"),
   TUniformProgramInput<cts("uValue"), TUniform<glm::tvec1>, tag::fragment>>;

auto progInputDef(cts("particlesys")) -> std::tuple<
   TNamedAttrib<cts("aPos"), TAttrib<glm::tvec3>>,
   TUniformProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4>>,
   TUniformProgramInput<cts("uSize"), TUniform<glm::tvec2>, tag::geometry>,
   TUniformProgramInput<cts("uPerspectiveScale"), TUniform<glm::tvec2>, tag::geometry>>;

auto progInputDef(cts("particlesys-tex")) -> ct::tuple_append<inp_by_name("particlesys"),
   TTextureProgramInput<cts("uTexture")>>;

auto progInputDef(cts("particlesys-tex-sprite")) -> ct::tuple_append<inp_by_name("particlesys-tex"),
   // aTime.x is age of particle, aTime.y is age of death
   TNamedAttrib<cts("aTime"), TAttrib<glm::tvec2>>,
   // for optimization purposes aSpeed.xyz is unit speed directon, aSpeed.w is
   // scalar speed value
   TNamedAttrib<cts("aSpeed"), TAttrib<glm::tvec4>>,
   TUniformProgramInput<cts("uAtlasSize"), TUniform<glm::tvec2, int, int>, tag::geometry>>;

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
