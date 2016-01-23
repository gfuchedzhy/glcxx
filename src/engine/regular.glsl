/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifdef TEX
DECL_VERT_FRAG(vec2, UV);
#endif

#if defined VERTEX

void main()
{
   gl_Position = uViewProj*(uModel*aPos);
#ifdef TEX
   out(UV) = aUV;
#endif
}

#elif defined FRAGMENT

void main()
{
#ifdef COL
   gl_FragColor = vec4(uColor, 1.0);
#endif
#ifdef TEX
   gl_FragColor = texture2D(uTexture, in(UV));
#endif
}

#endif
