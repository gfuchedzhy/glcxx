/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

varying vec3 vEyeDir;

#ifdef NMAP
varying vec3 vSunDir;
#else
varying vec3 vNorm;
#endif

#ifdef TEX
varying vec2 vUV;
#endif

#if defined VERTEX

void main()
{
   vec4 pos = uModel*aPos;
   gl_Position = uViewProj*pos;

#ifdef NMAP
   mat3 tbn = transpose(mat3(uModel)
                      * mat3(aTan, cross(aNorm, aTan), aNorm));
   vSunDir = tbn*uSunDir;
   vEyeDir = normalize(tbn*(uEye - pos.xyz));
#else
   vEyeDir = normalize(uEye - pos.xyz);
   vNorm = (uModel*vec4(aNorm, 0.0)).xyz;
#endif

#ifdef TEX
   vUV = aUV;
#endif
}

#elif defined FRAGMENT

const vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
const float diffuseIntensity = 0.7;
const float ambientIntensity = 0.5;
const float specularIntensity = 0.8;

void main()
{
#ifdef NMAP
   vec2 normXY = 2*texture2D(uNormalMap, vUV).xy - 1;
   vec3 norm = vec3(normXY.x, -normXY.y, sqrt(1-length(normXY)));
   vec3 geomNorm = vec3(0, 0, 1); // tbn space
   vec3 sunDirNorm = normalize(vSunDir);
#else
   vec3 norm = normalize(vNorm);
   vec3 geomNorm = norm;
   vec3 sunDirNorm = uSunDir;
#endif

   vec4 diffuse = lightColor*diffuseIntensity*clamp(dot(norm, sunDirNorm), 0, 1);
   vec4 ambient = lightColor*ambientIntensity;
   vec3 reflected = reflect(-sunDirNorm, norm);
   vec4 specular = step(0.0, dot(sunDirNorm, geomNorm)) * lightColor
      * specularIntensity * pow(clamp(dot(normalize(vEyeDir), reflected), 0, 1), 20);

#ifdef COL
   vec4 color = vec4(uColor, 1.0);
#endif
#ifdef TEX
   vec4 color = texture2D(uTexture, vUV);
#endif
   gl_FragColor = color * (ambient + diffuse) + specular;
}

#endif
