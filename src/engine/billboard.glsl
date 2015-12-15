/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifdef TEX
varying vec2 vUV;
#endif

#ifdef HB
varying vec2 vPos;
#endif

#if defined VERTEX

#ifdef HB
float yCoef = 1.3; // use 30% of uSize.y as offset
float xCoef = (yCoef - 1.0)*uSize.y/uSize.x + 1.0;
#endif

void main()
{
   gl_Position = uViewProj*vec4(uExternalPos + uPos + aPos.x*uSize.x*uRight + aPos.y*uSize.y*uUp, 1.0);

#if defined SPRITE
   vUV = vec2((aUV.x + uCurrentFrame)/uFrameNumber, aUV.y);
#elif defined TEX
   vUV = aUV;
#elif defined HB
   vPos = vec2(xCoef*aPos.x + 0.5,
               yCoef*aPos.y + 0.5);
#endif
}

#elif defined FRAGMENT

void main()
{
#if defined TEX
   gl_FragColor = texture2D(uTexture, vUV);
#elif defined HB
   vec2 stepRes = step(vec2(0.0, 0.0), vPos) - step(vec2(uValue, 1.0), vPos);
   float colorWeight = stepRes.x * stepRes.y;
   gl_FragColor = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), colorWeight);
#endif
}

#endif
