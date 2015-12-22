/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifdef TEX
varying vec2 vUV;
#endif

#if defined FLAME
varying vec4 vColor;
varying vec2 vUV2;
varying float mixFactor;
#endif

#if defined VERTEX

const vec2 squad[] = vec2[](vec2(-0.5,-0.5),
                            vec2( 0.5,-0.5),
                            vec2( 0.5, 0.5),
                            vec2(-0.5, 0.5));

const vec2 uv[] = vec2[](vec2(0.0, 1.0),
                         vec2(1.0, 1.0),
                         vec2(1.0, 0.0),
                         vec2(0.0, 0.0));

void main()
{
   int i = gl_VertexID % 4;

#if defined FLAME
   vColor.rgb = mix(vec3(0, 0.2, 1),
                    vec3(1, 0.6, 0),
                    min(1, aTime.x/0.06));
   vColor.rgb = mix(vColor.rgb,
                    vec3(1, 1, 1),
                    clamp((aTime.x - 0.05)/0.1, 0, 1));

   vColor.a = 1
      - smoothstep(0, 0.1, aTime.x)
      + 0.4*smoothstep(0.1, 0.3, aTime.x)
      - 0.4*smoothstep(0.3, aTime.y, aTime.x);

   float size = (1 + 2.5*aTime.x)
      * (1
         - smoothstep(0, 0.15, aTime.x)
         + 0.7*smoothstep(0.1, 0.15, aTime.x));

   // d2x/dt2 = - airResistance * (dx/dt)^2
   const float airResistance = 0.05;
   vec3 pos = aPos + aSpeed.xyz*log(1+airResistance*aSpeed.w*aTime.x)/airResistance;

   vec2 shift = squad[i]*uSize*size;
   gl_Position = uViewProj*vec4(pos + shift.x*uRight + shift.y*uUp, 1);
#else
   vec2 shift = squad[i]*uSize;
   gl_Position = uViewProj*vec4(aPos + shift.x*uRight + shift.y*uUp, 1);
#endif

#if defined FLAME
   // 3 fps with interfading, +aTime.y to randomize starting frame
   float floatFrame = 3*(aTime.x + aTime.y); // to randomize starting frame
   int frame = int(floatFrame);
   mixFactor = floatFrame - frame;
   vUV  = (uv[i] + vec2(frame%uAtlasSize.x, frame/uAtlasSize.y))/uAtlasSize;
   vUV2  = (uv[i] + vec2((frame+1)%uAtlasSize.x, (frame+1)/uAtlasSize.y))/uAtlasSize;
#elif defined SPRITE
   int frame = int(30*aTime.x);
   vUV  = (uv[i] + vec2(frame%uAtlasSize.x, frame/uAtlasSize.y))/uAtlasSize;
#elif defined TEX
   vUV = uv[i];
#endif
}

#elif defined FRAGMENT

void main()
{
#if defined FLAME
   gl_FragColor = mix(texture2D(uTexture, vUV),
                      texture2D(uTexture, vUV2),
                      mixFactor);
   gl_FragColor *= vColor;
#elif defined TEX
   gl_FragColor = texture2D(uTexture, vUV);
#endif

}

#endif
