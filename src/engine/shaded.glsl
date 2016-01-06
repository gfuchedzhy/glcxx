/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

DECL_VERT_FRAG(vec3, eyeDir);

#ifdef NMAP
DECL_VERT_FRAG(vec3, sunDir);
#else
DECL_VERT_FRAG(vec3, norm);
#endif

#ifdef TEX
DECL_VERT_FRAG(vec2, UV);
#endif

#if defined VERTEX

void main()
{
   vec4 pos = uModel*aPos;
   gl_Position = uViewProj*pos;

#ifdef NMAP
   mat3 tbn = transpose(mat3(uModel)
                      * mat3(aTan, cross(aNorm, aTan), aNorm));
   out(sunDir) = tbn*uSunDir;
   out(eyeDir) = normalize(tbn*(uEye - pos.xyz));
#else
   out(eyeDir) = normalize(uEye - pos.xyz);
   out(norm) = (uModel*vec4(aNorm, 0.0)).xyz;
#endif

#ifdef TEX
   out(UV) = aUV;
#endif
}

#elif defined FRAGMENT

const float diffuseIntensity = 1.3;
const float ambientIntensity = 3;
const float specularIntensity = 2;

void main()
{
#ifdef NMAP
   vec2 normXY = 2*texture2D(uNormalMap, in(UV)).xy - 1;
   vec3 norm = vec3(normXY.x, -normXY.y, sqrt(1-length(normXY)));
   vec3 geomNorm = vec3(0, 0, 1); // tbn space
   vec3 sunDirNorm = normalize(in(sunDir));
#else
   vec3 norm = normalize(in(norm));
   vec3 geomNorm = norm;
   vec3 sunDirNorm = uSunDir;
#endif

   vec4 diffuse = vec4(uDiffuse, 1.0)*diffuseIntensity*clamp(dot(norm, sunDirNorm), 0, 1);
   vec4 ambient = vec4(uAmbient, 1.0)*ambientIntensity*clamp(dot(norm, geomNorm), 0, 1);
   vec3 reflected = reflect(-sunDirNorm, norm);
   vec4 specular = step(0.0, dot(sunDirNorm, geomNorm)) * vec4(uSpecular, 1.0)
      * specularIntensity * pow(clamp(dot(normalize(in(eyeDir)), reflected), 0, 1), uShininess);

#ifdef COL
   vec4 color = vec4(uColor, 1.0);
#endif
#ifdef TEX
   vec4 color = texture2D(uTexture, in(UV));
#endif
   gl_FragColor = color * (ambient + diffuse + specular/color.w);
}

#endif
