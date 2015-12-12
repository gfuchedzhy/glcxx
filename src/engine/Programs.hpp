/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMS_HPP
#define ENGINE_PROGRAMS_HPP

#include "Program.hpp"
#include "Texture.hpp"

inline auto make_program(cts("colored"))
{
   return std::make_unique<TProgram<TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
                                    TProgramInput<cts("uModel"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uColor"), TUniform<glm::tvec3, float>, tag::fragment>>
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

inline auto make_program(cts("coloredIlluminated"))
{
   return std::make_unique<TProgram<TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
                                    TProgramInput<cts("aNorm"), TAttrib<glm::tvec3, float>>,
                                    TProgramInput<cts("uModel"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uSunDir"), TUniform<glm::tvec3, float>, tag::fragment>,
                                    TProgramInput<cts("uEye"), TUniform<glm::tvec3, float>, tag::fragment>,
                                    TProgramInput<cts("uColor"), TUniform<glm::tvec3, float>, tag::fragment>>
                           >(R"(\
varying vec3 vPos;
varying vec3 vNorm;
void main()
{
   gl_Position = uViewProj*uModel*aPos;
   vPos = (uModel*aPos).xyz;
   vNorm = (uModel*vec4(aNorm, 0.0)).xyz;
})",
                             R"(\
const vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
const float diffuseIntensity = 0.7;
const float ambientIntensity = 0.5;
const float specularIntensity = 0.8;

varying vec3 vPos;
varying vec3 vNorm;
void main()
{
   vec3 norm = normalize(vNorm);
   vec4 diffuse = lightColor*diffuseIntensity*clamp(dot(norm, uSunDir), 0, 1);
   vec4 ambient = lightColor*ambientIntensity;
   vec3 reflected = reflect(-uSunDir, norm);
   vec3 cameraDir = normalize(uEye - vPos);
   vec4 specular = lightColor*specularIntensity*pow(clamp(dot(cameraDir, reflected), 0, 1), 20);

   gl_FragColor = vec4(uColor, 1.0) * (ambient + diffuse) + specular;
})");
}

inline auto make_program(cts("texturedIlluminated"))
{
   return std::make_unique<TProgram<TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
                                    TProgramInput<cts("aNorm"), TAttrib<glm::tvec3, float>>,
                                    TProgramInput<cts("aTan"), TAttrib<glm::tvec3, float>>,
                                    TProgramInput<cts("aUV"), TAttrib<glm::tvec2, float>>,
                                    TProgramInput<cts("uModel"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uSunDir"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uEye"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>,
                                    TProgramInput<cts("uNormalMap"), TTextureProgramInput<1>, tag::fragment>>
                           >(R"(\
varying vec3 vEyeDir;
varying vec3 vSunDir;
varying vec2 vUV;
void main()
{
   vec4 pos = uModel*aPos;
   gl_Position = uViewProj*pos;

   mat3 tbn = transpose(mat3(uModel)
                      * mat3(aTan, cross(aNorm, aTan), aNorm));

   vSunDir = tbn*uSunDir;
   vEyeDir = normalize(tbn*(uEye - pos.xyz));
   vUV = aUV;
})",
                             R"(\
const vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
const float diffuseIntensity = 0.7;
const float ambientIntensity = 0.5;
const float specularIntensity = 0.8;

varying vec3 vEyeDir;
varying vec3 vSunDir;
varying vec2 vUV;
void main()
{
   vec2 normXY = 2*texture2D(uNormalMap, vUV).xy - 1;
   vec3 norm = vec3(normXY.x, -normXY.y, sqrt(1-length(normXY)));
   vec3 sunDirNorm = normalize(vSunDir);
   vec4 diffuse = lightColor*diffuseIntensity*clamp(dot(norm, sunDirNorm), 0, 1);
   vec4 ambient = lightColor*ambientIntensity;
   vec3 reflected = reflect(-sunDirNorm, norm);
   // sunDirNorm.z is essentially dot(triangle_normal, sunDirNorm), but as we
   // are in tbn space triangle_normal=vec3(0, 0, 1)
   vec4 specular = step(0.0, sunDirNorm.z) * lightColor*specularIntensity*pow(clamp(dot(normalize(vEyeDir), reflected), 0, 1), 20);

   gl_FragColor = texture2D(uTexture, vUV) * (ambient + diffuse) + specular;
})");

}

inline auto make_program(cts("texturedPolygon"))
{
   return std::make_unique<TProgram<TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
                                    TProgramInput<cts("aUV"), TAttrib<glm::tvec2, float>>,
                                    TProgramInput<cts("uModel"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>>
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
   return std::make_unique<TProgram<TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
                                    TProgramInput<cts("aUV"), TAttrib<glm::tvec2, float>>,
                                    TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uPos"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uExternalPos"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uSize"), TUniform<glm::tvec2, float>>,
                                    TProgramInput<cts("uRight"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uUp"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>>
                           >(R"(\
varying vec2 vUV;
void main()
{
   gl_Position = uViewProj*vec4(uPos + uExternalPos + aPos.x*uSize.x*uRight + aPos.y*uSize.y*uUp, 1.0);
   vUV = aUV;
})",
                             R"(\
varying vec2 vUV;
void main()
{
   gl_FragColor = texture2D(uTexture, vUV);
})");
}

inline auto make_program(cts("animationObject"))
{
   return std::make_unique<TProgram<TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
                                    TProgramInput<cts("aUV"), TAttrib<glm::tvec2, float>>,
                                    TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uPos"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uCurrentFrame"), TUniform<glm::tvec1, int>>,
                                    TProgramInput<cts("uFrameNumber"), TUniform<glm::tvec1, int>>,
                                    TProgramInput<cts("uSize"), TUniform<glm::tvec2, float>>,
                                    TProgramInput<cts("uRightStabilized"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uTexture"), TTextureProgramInput<>, tag::fragment>>
                           >(R"(\
const vec3 up = vec3(0.0, 0.0, 1.0);
varying vec2 vUV;
void main()
{
   gl_Position = uViewProj*vec4(uPos + aPos.x*uSize.x*uRightStabilized + aPos.y*uSize.y*up, 1.0);
   vUV = vec2((aUV.x + uCurrentFrame)/uFrameNumber, aUV.y);
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
   return std::make_unique<TProgram<TProgramInput<cts("aPos"), TAttrib<glm::tvec3, float, float, 1>>,
                                    TProgramInput<cts("uViewProj"), TUniform<glm::tmat4x4, float>>,
                                    TProgramInput<cts("uPos"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uExternalPos"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uSize"), TUniform<glm::tvec2, float>>,
                                    TProgramInput<cts("uRight"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uUp"), TUniform<glm::tvec3, float>>,
                                    TProgramInput<cts("uValue"), TUniform<glm::tvec1, float>, tag::fragment>>
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
                            cts("coloredIlluminated"),
                            cts("texturedIlluminated"),
                            cts("texturedPolygon"),
                            cts("texturedBillboard"),
                            cts("animationObject"),
                            cts("healthbar")>;
extern tRenderer gRenderer;

#endif // ENGINE_PROGRAMS_HPP
