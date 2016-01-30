/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifdef TEX
DECL_VERT_FRAG(vec2, UV);
#endif

#ifdef HB
DECL_VERT_FRAG(vec2, pos);
#endif

#if defined VERTEX

#ifdef HB
float yCoef = 1.3; // use 30% of uSize.y as offset
float xCoef = (yCoef - 1.0)*uSize.y/uSize.x + 1.0;
#endif

void main()
{
   vec3 right = normalize(cross(uUp, uEyePos - uPos));
   gl_Position = uViewProj*vec4(uPos + aPos.x*uSize.x*right + aPos.y*uSize.y*uUp, 1.0);

#if defined SPRITE
   out(UV) = (aUV + uAtlasPos)/uAtlasSize;
#elif defined TEX
   out(UV) = aUV;
#elif defined HB
   out(pos) = vec2(xCoef*aPos.x + 0.5,
                   yCoef*aPos.y + 0.5);
#endif
}

#elif defined FRAGMENT

void main()
{
#if defined TEX
   gl_FragColor = texture2D(uTexture, in(UV));
#elif defined HB
   vec2 stepRes = step(vec2(0.0, 0.0), in(pos)) - step(vec2(uValue, 1.0), in(pos));
   float colorWeight = stepRes.x * stepRes.y;
   gl_FragColor = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), colorWeight);
#endif
}

#endif
