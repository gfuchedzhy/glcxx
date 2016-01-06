/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifdef TEX
DECL_GEOM_FRAG(vec2, UV);
#endif

#if defined FLAME
DECL_VERT_GEOM(vec4, color);
DECL_VERT_GEOM(float, size);
DECL_VERT_GEOM(float, frame);

DECL_GEOM_FRAG(vec4, color);
DECL_GEOM_FRAG(vec2, UV2);
DECL_GEOM_FRAG(float, mixFactor);

#elif defined SPRITE
DECL_VERT_GEOM(int, frame);
#endif

#if defined VERTEX

void main()
{
#ifdef FLAME
   out(color).rgb = mix(vec3(0, 0.2, 1),
                        vec3(1, 0.6, 0),
                        min(1, aTime.x/0.06));
   out(color).rgb = mix(out(color).rgb,
                        vec3(1, 1, 1),
                        clamp((aTime.x - 0.05)/0.1, 0, 1));

   out(color).a = 1
      - smoothstep(0, 0.1, aTime.x)
      + 0.4*smoothstep(0.1, 0.3, aTime.x)
      - 0.4*smoothstep(0.3, aTime.y, aTime.x);

   out(size) = (1 + 2.5*aTime.x)
      * (1
         - smoothstep(0, 0.15, aTime.x)
         + 0.7*smoothstep(0.1, 0.15, aTime.x));

   // d2x/dt2 = - airResistance * (dx/dt)^2
   const float airResistance = 0.05;
   vec3 pos = aPos + aSpeed.xyz*log(1+airResistance*aSpeed.w*aTime.x)/airResistance;
   gl_Position = uViewProj*vec4(pos, 1);
#else
   gl_Position = uViewProj*vec4(aPos, 1);
#endif

#ifdef FLAME
   // 3 fps with interfading, +aTime.y to randomize starting frame
   out(frame) = 3*(aTime.x + aTime.y);
#elif defined SPRITE
   out(frame) = int(30*aTime.x);
#endif
}

#elif defined GEOMETRY

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

const vec2 squad[] = vec2[](vec2(-0.5,-0.5),
                            vec2( 0.5,-0.5),
                            vec2(-0.5, 0.5),
                            vec2( 0.5, 0.5));

const vec2 uv[] = vec2[](vec2(0.0, 1.0),
                         vec2(1.0, 1.0),
                         vec2(0.0, 0.0),
                         vec2(1.0, 0.0));

void main()
{
   for (int i = 0; i < 4; i++)
   {
#ifdef FLAME
      out(color) = in(color)[0];
      int frame = int(in(frame)[0]);
      out(UV)  = (uv[i] + vec2(frame%uAtlasSize.x, frame/uAtlasSize.y))/uAtlasSize;
      out(UV2) = (uv[i] + vec2((frame+1)%uAtlasSize.x, (frame+1)/uAtlasSize.y))/uAtlasSize;
      out(mixFactor) = in(frame)[0] - frame;
#elif defined SPRITE
      out(UV) = (uv[i] + vec2(out(frame)[0]%uAtlasSize.x, out(frame)[0]/uAtlasSize.y))/uAtlasSize;
#elif defined TEX
      out(UV) = uv[i];
#endif
      gl_Position = gl_in[0].gl_Position;

      // expand billboard in projective space
      vec2 shift = squad[i]*uPerspectiveScale*uSize;
#ifdef FLAME
      shift *= in(size)[0];
#endif
      gl_Position.xy += shift;
      EmitVertex();
   }
   EndPrimitive();
}

#elif defined FRAGMENT

void main()
{
#if defined FLAME
   gl_FragColor = mix(texture2D(uTexture, in(UV) ),
                      texture2D(uTexture, in(UV2) ),
                      in(mixFactor));
   gl_FragColor *= in(color);
#elif defined TEX
   gl_FragColor = texture2D(uTexture, vgUV);
#endif
}

#endif
