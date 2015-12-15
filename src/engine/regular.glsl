/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifdef TEX
varying vec2 vUV;
#endif

#if defined VERTEX

void main()
{
   gl_Position = uViewProj*uModel*aPos;
#ifdef TEX
   vUV = aUV;
#endif
}

#elif defined FRAGMENT

void main()
{
#ifdef COL
   gl_FragColor = vec4(uColor, 1.0);
#endif
#ifdef TEX
   gl_FragColor = texture2D(uTexture, vUV);
#endif
}

#endif
