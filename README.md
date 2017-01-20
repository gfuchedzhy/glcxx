# glcxx

C++ wrapper library for OpenGL that enforces compile-time type safety on
communication with OpenGL and simplifies its usage a lot.

This library is very experimental. I wanted to explore some advanced C++
techniques in context of OpenGL applications. And this is what came out of it.

## Overview by example
### Basics
You have to understand how OpenGL works, what vbo, vao and other OpenGL concepts
are in order to understand what is the point of this library.

Some sidenote first. Can you make compile-time constant, similar to
```std::integral_constant```, but carrying string in C++? Well, sort of...
But how exactly to do that is story for another time. For now, we need to
know that this: ```cts("somestring")``` is compile-time string. It is a type.
Just like ```std::integral_constant<..>```, but it contains a string. We
will use it to dispatch method calls at compile-time using human-readable
strings.

Let's draw a rectangle! You've already created window, context, and all that, have
you? Let's draw then!

```c++
// we need a program, that will take vertex positions
using simple_program = glcxx::program<
    // this program has one attribute, named pos, that is vec3
    glcxx::vao_input<cts("pos"), glm::vec3>
>;

// we will put this program into renderer,
// which essentially is a manager of our programs
using renderer_t = glcxx::renderer<
    // we will call our program "simple"
    std::pair<cts("simple"), simple_program>
>;

// create instance of renderer
renderer_t renderer;
```

Let's now create a simple.glsl file with our simple shaders:
```c
#if defined VERTEX
void main() {
    gl_Position = vec4(pos, 1.0);
}
#elif defined FRAGMENT
void main() {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
#endif
```

Notice, there's no declaration of pos attribute. We've already declared it in
C++ code! glcxx will take care of it, as well as of all other attributes and
uniforms.

We will keep both vertex and fragment shaders in single file separated from each
other using preprocessor directive.

Compile it into binary using cmake
```cmake
glcxx_compile_shaders(shaders simple.glsl)
```
Just link static library 'shaders' to your binary and you are done.

Now let's create vertices and draw!
```c++
// this is vao object, that carries one vec3 attribute named "pos"
glcxx::vao<std::pair<cts("pos"), glm::vec3>> pos_vao;

const glm::vec3 positions[] = {
    {-0.5f,-0.5f, 0.f},
    { 0.5f,-0.5f, 0.f},
    {-0.5f, 0.5f, 0.f},
    { 0.5f, 0.5f, 0.f}
};

// strip of 2 triangles, of course, we can draw this rectangle without
// indices at all, using arrays draw call, but for the sake of example,
// let's try both ways
const GLubyte indices[] = {0, 1, 2, 3};

// create vertex buffer, attach it to vao and upload data to GPU
// this method will accept static array, vector or pointer and size
// of appropriate type
pos_vao.upload<cts("pos")>(positions);

// create index buffer, attach it to vao and upload data to GPU,
// this method will accept indices of any size (GLubyte, GLushort, GLuint)
// it will deduce appropriate OpenGL enum values (like GL_UNSIGNED_BYTE)
// to use in OpenGL calls for you, so you don't have to worry about it.
pos_vao.upload_indices(indices, GL_TRIANGLE_STRIP);

// retrieve our program by name, don't worry,
// there's no string search or things like that,
// everything is resolved at compile time,
// if there's no such program, we'll get compilation error
auto& simple_program = renderer.program<cts("simple")>();

// draw using uploaded indices
simple_program.draw_elements(pos_vao);

// or draw without indices, uploaded indices are ignored
simple_program.draw_arrays(pos_vao, GL_TRIANGLE_STRIP, 0, 4);
```

There's rectangle on the screen!

![white rectangle](/img/white.png?raw=true)

Let's make another program that has configurable color.
```c++
// we will derive program from previous one, why repeat yourself,
// if our program is just like previous one but with color?
using simple_colored = glcxx::derive_program<
    simple_program,
    // add fragment uniform called "color" of type vec4
    glcxx::uniform<cts("color"), glm::vec4, glcxx::tag::fragment>
>;

// renderer now has 2 programs
using renderer_t = glcxx::renderer<
    std::pair<cts("simple"), simple_program>,
    std::pair<cts("simple_colored"), simple_colored>
>;

// draw
auto& colored = renderer.program<cts("simple_colored")>();
colored.set<cts("color")>({1, 0, 0, 1}); // set uniform, named "color"
colored.draw_elements(pos_vao);
```
Let's extend our shader. Derived programs are similar, so they share same source
file. Notice COLORED preprocessor variable, it will be set automatically for you
by glcxx. Thanks to compile-time strings!

```c
#if defined VERTEX
void main() {
    gl_Position = vec4(pos, 1.0);
}
#elif defined FRAGMENT
void main() {
#if defined COLORED
    gl_FragColor = color;
#else
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
#endif
}
#endif
```

Now it looks like this:

![red rectangle](/img/red.png?raw=true)


What will happen if we decide to change uniform from vec4 to vec3? We'll get
compilation error, and will be forced to change

```c++
colored.set<cts("color")>({1, 0, 0, 1});
```
to
```c++
colored.set<cts("color")>({1, 0, 0});
```

So, here's typical task, you want new uniform for your program. What do you do?
You add it to program definition, so that glcxx knows about it. You use it in
shader itself, no need to define it there. You set it before draw using
appropriate set function that is already created for you. That's it!

Let's now add different color for each vertex.
```c++
// new program
using simple_colored_attr = glcxx::derive_program<
    simple_program,
    // new attribute for colors
    glcxx::vao_input<cts("color"), glm::vec4>
>;

// add third program to renderer
using renderer_t = glcxx::renderer<
    std::pair<cts("simple"), simple_program>,
    std::pair<cts("simple_colored"), simple_colored>,
    std::pair<cts("simple_colored_attr"), simple_colored_attr>
>;

// vao for 2 attributes
glcxx::vao<std::pair<cts("pos"), glm::vec3>,
           std::pair<cts("color"), glm::vec4>> pos_color_vao;

// let's share index buffer and position buffer between vaos,
// vao takes shared ownership of buffers, index_buf and pos_buf are shared_ptrs
auto index_buf = glcxx::make_index_buffer(indices, GL_TRIANGLE_STRIP);
pos_vao.indices(index_buf);
pos_color_vao.indices(index_buf);

auto pos_buf = glcxx::make_buffer(positions);
pos_vao.set<cts("pos")>(pos_buf);
pos_color_vao.set<cts("pos")>(pos_buf);

// upload colors
const glm::vec4 colors[] = {
    {1.f, 0.f, 0.f, 1.f},
    {0.f, 1.f, 0.f, 1.f},
    {1.f, 1.f, 1.f, 1.f},
    {0.f, 0.f, 1.f, 1.f}
};
pos_color_vao.upload<cts("color")>(colors);

// draw
auto& colored_attr = renderer.program<cts("simple_colored_attr")>();
colored_attr.draw_elements(pos_color_vao);
```

Shader:
```c
#if defined VERTEX
#if defined ATTR
out vec4 v_color;
#endif
void main() {
    gl_Position = vec4(pos, 1.0);
#if defined ATTR
    v_color = color;
#endif
}
#elif defined FRAGMENT
#if defined ATTR
in vec4 v_color;
#endif
void main() {
#if defined ATTR
    gl_FragColor = v_color;
#elif defined COLORED
    gl_FragColor = color;
#else
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
#endif
}
#endif
```

Now it looks like this:

![multicolor rectangle](/img/multicolor.png?raw=true)

Actually, we can use second vao for first two programs too. Redundant attribute
just ignored in this case.

```c++
auto& simple = renderer.program<cts("simple")>();
simple.draw_elements(pos_color_vao);

auto& colored = renderer.program<cts("simple_colored")>();
colored.set<cts("color")>({1, 0, 0, 1});
colored.draw_elements(pos_color_vao);
```

But, if we try the other way around, we will get compilation error. Because
compiler knows that pos_vao doesn't match all attributes required for that
program..

```c++
auto& colored_attr = renderer.program<cts("simple_colored_attr")>();
colored_attr.draw_elements(pos_vao); // compilation error!
```

Let's now try to use interleaved data layout for our attributes.
```c++
struct pos_color_t {
    glm::vec3 pos;
    glm::vec4 color;
};
const pos_color_t pos_color[] = {
    { {-0.5f,-0.5f, 0.f}, {1.f, 0.f, 0.f, 1.f} },
    { { 0.5f,-0.5f, 0.f}, {0.f, 1.f, 0.f, 1.f} },
    { {-0.5f, 0.5f, 0.f}, {1.f, 1.f, 1.f, 1.f} },
    { { 0.5f, 0.5f, 0.f}, {0.f, 0.f, 1.f, 1.f} }
};

auto pos_color_buf = glcxx::make_buffer(pos_color);
pos_color_vao.set<cts("pos")>(pos_color_buf, &pos_color_t::pos);
pos_color_vao.set<cts("color")>(pos_color_buf, &pos_color_t::color);
```

See, how easy this is? No void pointers, no offsets calculated by hand, no
frustrating crashes :)

Programs are created when the renderer is created. glcxx will compile and link
programs for you. It will throw exception with error messages from video driver
if any. Then for each uniform and each attribute location ID would be requested
and saved. For each uniform current value also gets saved.

Let's say we have such program:
```c++
using myprogram_t = glcxx::program<
    glcxx::vao_input<cts("pos"), glm::vec3>,
    glcxx::vao_input<cts("uv"), glm::vec2>,
    glcxx::vao_input<cts("normal"), glm::vec3>,
    glcxx::uniform<cts("color"), glm::vec4, glcxx::tag::fragment>,
    glcxx::uniform<cts("transformation"), glm::mat4, glcxx::tag::vertex>,
    glcxx::texture_input<cts("tex")>
>;
```

What happens exactly, when we make following call?
```c++
myprogram.set<cts("color")>(newcolor);
```

```set<cts("color")>``` gets resolved at compile time to appropriate tuple
element containing location ID and current uniform value set in program.

So that call essentially gets compiled into something like this:
```c++
if (newcolor != current_color_uniform_value) {
    current_color_uniform_value = newcolor;
    glUniform4f(color_uniform_location_id, newcolor.r, newcolor.g, newcolor.b, newcolor.a);
}
```

So caching, compile-time resolution of uniform string name and selection of
proper ```glUniform*``` function is done for us automatically.

What about attributes?
```c++
myprogram.draw_elements(myvao);
```

When we draw ```myvao``` with ```myprogram``` for the first time, appropriate
```glVertexAttrib*Pointer(...)``` function call is made for each
attribute. glcxx have all the necessary information to select proper function
from ```glVertexAttrib*Pointer``` family at compile-time, set proper offsets,
stride, type, etc. Interleaved data layout or separate buffers, or some mix of
both, glcxx will perform all that tedious and error-prone work for us. All
parameters are strictly typed. Again, no more void pointers and frustrating
crashes :)

When ```myvao``` was already drawn with ```myprogram``` once, next time it will
just bind vao and issue a draw call.

### Instancing
What about something a little more advanced? Maybe instanced draw? Let's draw
rectangle of distinct color at every current vertex position using instancing.

```c++
using simple_colored_attr = glcxx::derive_program<
    simple_program,
    glcxx::vao_input<cts("color"), glm::vec4>,
    // we need one more attribute for dimensions of our rectangles
    glcxx::vao_input<cts("size"), glm::vec2>
>;

// new vao that carries all attributes
glcxx::vao<std::pair<cts("pos"), glm::vec3>,
           std::pair<cts("color"), glm::vec4>,
           std::pair<cts("size"), glm::vec2>> instanced_vao;

// same index buffer
instanced_vao.indices(index_buf);

// same interleaved positions and colors, but notice 1 as last parameter,
// that is divisor, which means advance 1 position/color per each instance
instanced_vao.set<cts("pos")>(pos_color_buf, &pos_color_t::pos, 1);
instanced_vao.set<cts("color")>(pos_color_buf, &pos_color_t::color, 1);

// there are 4 rectangles to draw
instanced_vao.instance_count(4);

// size of each rectangle
const glm::vec2 size[] = {
    {-0.2f,-0.2f},
    { 0.2f,-0.2f},
    {-0.2f, 0.2f},
    { 0.2f, 0.2f}
};
instanced_vao.set<cts("size")>(glcxx::make_buffer(size));
```

Shader:
```c
...
#if defined ATTR
    gl_Position.xy += size; // add this line
    v_color = color;
#endif
...
```
Draw:
```c++
// draw, same function to draw, beacause vao object already
// knows that we are using instancing
auto& colored_attr = renderer.program<cts("simple_colored_attr")>();
colored_attr.draw_elements(instanced_vao);
```

4 rectangles of different colors!

![4 rectangles](/img/multiple-distinct-colors.png?raw=true)

How about 4 rectangles with each vertex of different color?
```c++
// just remove divisor for colors!
instanced_vao.set<cts("color")>(pos_color_buf, &pos_color_t::color);
```

![4 rectangles](/img/multiple-multicolor.png?raw=true)

### Geometry shader
Let's do same thing with geometry shader. We will change our third program again
to draw same 4 squares but using geometry shader instead of instancing.

```c++
using simple_colored_attr = glcxx::derive_program<
    simple_program,
    glcxx::vao_input<cts("color"), glm::vec4>,

    // we could use vec2 as a uniform for size, but for the sake of example,
    // we will use uniform array of vec2 for bottom-left and top-right vertex
    glcxx::uniform<cts("size"), std::array<glm::vec2, 4>, glcxx::tag::geometry>
>;
```

Hey, we added uniform for geometry stage of our program, so glcxx will compile
and link geometry shader into our program. That's all you have to do to enable
geometry shader! In case you want geometry shader which happen to not require
any unifroms, just list ```glcxx::tag::geometry``` among program inputs, like
this:

```c++
using simple_colored_attr = glcxx::derive_program<
    simple_program,
    glcxx::vao_input<cts("color"), glm::vec4>,
    glcxx::tag::geometry
>;
```

But where were we? We need to modify shader itself. We'll add new section to
simple.glsl file.

```c
#if defined VERTEX
#if defined ATTR
out vec4 v_color_geom;
#endif
void main()
{
    gl_Position = vec4(pos, 1.0);
#if defined ATTR
    v_color_geom = color;
#endif
}
#elif defined GEOMETRY
in vec4 v_color_geom[];
out vec4 v_color;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

void main() {
    for (int y = 0; y < 2; y++)
        for (int x = 0; x < 2; x++) {
            v_color = v_color_geom[0];
            gl_Position = gl_in[0].gl_Position;
            gl_Position.x += size[x].x;
            gl_Position.y += size[y].y;
            EmitVertex(); // emit vertex for every corner of rectangle
        }
    EndPrimitive();
}

#elif defined FRAGMENT
// fragment shader stays the same
...
```

We will use our old vao ```pos_color_vao```, but we want to draw with
```GL_POINTS``` instead of ```GL_TRIANGLE_STRIP```.

```c++
auto& colored_attr = renderer.program<cts("simple_colored_attr")>();
// set rectangle size uniform
colored_attr.set<cts("size")>({{{-0.2f,-0.2f}, {0.2f, 0.2f}}});
colored_attr.draw_arrays(pos_color_vao, GL_POINTS, 0, 4);
```

That's it! You just mentioned to glcxx that you want geometry shader and you
have it. You want uniform array? glcxx will handle all the boilerplate, like
what ```glUniform*``` function to call how many locations will this uniform take
and all that at compile-time.
