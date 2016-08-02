/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_INPUT_VAR_HPP
#define GLCXX_SRC_INPUT_VAR_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gl.hpp"
#include "utils.hpp"
#include "except.hpp"

namespace glcxx
{
   /// @brief functions to attach uniforms, replace huge amount of different c
   /// functions with properly overloaded fucntions, as their ammount is huge,
   /// use macro to reduce repitition
   #define DECLARE_ATTACH_UNIFORM_FUNCTIONS(type, functionSuffix)       \
   inline void attach_uniform(GLint location, type val)                 \
   {                                                                    \
      glcxx_gl(glUniform1##functionSuffix, location, val);              \
   }                                                                    \
   template<glm::precision P>                                           \
   inline void attach_uniform(GLint location, const glm::tvec2<type, P>& val) \
   {                                                                    \
      glcxx_gl(glUniform2##functionSuffix, location, val.x, val.y);     \
   }                                                                    \
   template<glm::precision P>                                           \
   inline void attach_uniform(GLint location, const glm::tvec3<type, P>& val) \
   {                                                                    \
      glcxx_gl(glUniform3##functionSuffix, location, val.x, val.y, val.z); \
   }                                                                    \
   template<glm::precision P>                                           \
   inline void attach_uniform(GLint location, const glm::tvec4<type, P>& val) \
   {                                                                    \
      glcxx_gl(glUniform4##functionSuffix, location, val.x, val.y, val.z, val.w); \
   }                                                                    \
   template<size_t N>                                                   \
   inline void attach_uniform(GLint location, const std::array<type, N>& val) \
   {                                                                    \
      glcxx_gl(glUniform1##functionSuffix##v, location, N, glm::value_ptr(val[0])); \
   }                                                                    \
   template<size_t N, glm::precision P>                                 \
   inline void attach_uniform(GLint location, const std::array<glm::tvec2<type, P>, N>& val) \
   {                                                                    \
      glcxx_gl(glUniform2##functionSuffix##v, location, N, glm::value_ptr(val[0])); \
   }                                                                    \
   template<size_t N, glm::precision P>                                 \
   inline void attach_uniform(GLint location, const std::array<glm::tvec3<type, P>, N>& val) \
   {                                                                    \
      glcxx_gl(glUniform3##functionSuffix##v, location, glm::value_ptr(val[0])); \
   }                                                                    \
   template<size_t N, glm::precision P>                                 \
   inline void attach_uniform(GLint location, const std::array<glm::tvec4<type, P>, N>& val) \
   {                                                                    \
      glcxx_gl(glUniform4##functionSuffix##v, location, glm::value_ptr(val[0])); \
   }

   /// @brief actual function definitions
   DECLARE_ATTACH_UNIFORM_FUNCTIONS(float, f);
   DECLARE_ATTACH_UNIFORM_FUNCTIONS(int, i);
   DECLARE_ATTACH_UNIFORM_FUNCTIONS(unsigned int, ui);

   /// @brief overloads for matrices
   template<glm::precision P>
   inline void attach_uniform(GLint location, const glm::tmat4x4<float, P>& val)
   {
      glcxx_gl(glUniformMatrix4fv, location, 1, GL_FALSE, glm::value_ptr(val));
   }
   template<size_t N, glm::precision P>
   inline void attach_uniform(GLint location, const std::array<glm::tmat4x4<float, P>, N>& val)
   {
      glcxx_gl(glUniformMatrix4fv, location, N, GL_FALSE, glm::value_ptr(val[0]));
   }

   /// @brief get uniform location
   inline auto get_uniform_loc(GLuint program, const char* name)
   {
      const auto location = glcxx_gl(glGetUniformLocation, program, name);
      if (-1 == location)
         throw input_location_error(std::string("uniform ") + name + " location wasn't found");
      return location;
   }

   /// @brief get attribute location
   inline auto get_attrib_loc(GLuint program, const char* name)
   {
      const auto location = glcxx_gl(glGetAttribLocation, program, name);
      if (-1 == location)
         throw input_location_error(std::string("attribute ") + name + " location wasn't found");
      return location;
   }

   /// @brief tags to place declarations to proper shaders
   namespace tag
   {
      struct vertex;
      struct geometry;
      struct fragment;
      struct vertgeom;
      struct geomfrag;
      struct vertfrag;
      struct all;
   }

   /// @brief return true if shader of type ShaderTag has declaration with
   /// DeclTag declaration tag
   template<typename ShaderTag, typename DeclTag> struct shader_has_decl;
   template<typename DeclTag>
   struct shader_has_decl<tag::vertex, DeclTag>
   {
         static constexpr bool value = ct::tuple_contains<std::tuple<tag::vertex, tag::vertgeom, tag::vertfrag, tag::all>,
                                                          DeclTag>::value;
   };
   template<typename DeclTag>
   struct shader_has_decl<tag::geometry, DeclTag>
   {
         static constexpr bool value = ct::tuple_contains<std::tuple<tag::geometry, tag::vertgeom, tag::geomfrag, tag::all>,
                                                          DeclTag>::value;
   };
   template<typename DeclTag>
   struct shader_has_decl<tag::fragment, DeclTag>
   {
         static constexpr bool value = ct::tuple_contains<std::tuple<tag::fragment, tag::vertfrag, tag::geomfrag, tag::all>,
                                                          DeclTag>::value;
   };

   namespace glsl
   {
      /// @brief compile time conversion from c++ types to glsl types
      template<typename T> struct type_id {};
      template<> struct type_id<float>          { static constexpr GLenum value = GL_FLOAT; };
      template<> struct type_id<double>         { static constexpr GLenum value = GL_DOUBLE; };
      template<> struct type_id<signed char>    { static constexpr GLenum value = GL_BYTE; };
      template<> struct type_id<unsigned char>  { static constexpr GLenum value = GL_UNSIGNED_BYTE; };
      template<> struct type_id<signed short>   { static constexpr GLenum value = GL_SHORT; };
      template<> struct type_id<unsigned short> { static constexpr GLenum value = GL_UNSIGNED_SHORT; };
      template<> struct type_id<signed int>     { static constexpr GLenum value = GL_INT; };
      template<> struct type_id<unsigned int>   { static constexpr GLenum value = GL_UNSIGNED_INT; };

      /// @brief compile time glsl prefixes for glsl
      namespace detail {
         template<typename T> struct type_prefix;
         template<> struct type_prefix<bool>         { using type = cts("b");};
         template<> struct type_prefix<int>          { using type = cts("i");};
         template<> struct type_prefix<unsigned int> { using type = cts("u");};
         template<> struct type_prefix<float>        { using type = cts(""); };
         template<> struct type_prefix<double>       { using type = cts("d");};
      }
      template<typename T> using type_prefix = typename detail::type_prefix<T>::type;

      /// @brief basic glsl type names
      namespace detail {
         template<typename T> struct basic_type_name;
         template<> struct basic_type_name<bool>         { using type = cts("bool");};
         template<> struct basic_type_name<int>          { using type = cts("int");};
         template<> struct basic_type_name<unsigned int> { using type = cts("uint");};
         template<> struct basic_type_name<float>        { using type = cts("float");};
         template<> struct basic_type_name<double>       { using type = cts("double");};
      }
      template<typename T> using basic_type_name = typename detail::basic_type_name<T>::type;

      /// @brief traits for glsl types
      template<typename TypeTo, size_t EXTRA, template<typename, glm::precision> class THolder>
      struct type_traits;

      template<typename TypeTo, size_t EXTRA>
      struct type_traits<TypeTo, EXTRA, glm::tvec1>
      {
            static constexpr size_t components_num = 1u;
            static constexpr size_t locations_num = 1u;
            static_assert(EXTRA <= 3, "too many extra components given");
            using type_name = typename std::conditional<0==EXTRA,
                                                        basic_type_name<TypeTo>,
                                                        ct::string_cat<type_prefix<TypeTo>, cts("vec"), ct::string<'1'+EXTRA>>>::type;
      };

      template<typename TypeTo, size_t EXTRA>
      struct type_traits<TypeTo, EXTRA, glm::tvec2>
      {
            static constexpr size_t components_num = 2u;
            static constexpr size_t locations_num = 1u;
            static_assert(EXTRA <= 2, "too many extra components given");
            using type_name = ct::string_cat<type_prefix<TypeTo>, cts("vec"), ct::string<'2'+EXTRA>>;
      };

      template<typename TypeTo, size_t EXTRA>
      struct type_traits<TypeTo, EXTRA, glm::tvec3>
      {
            static constexpr size_t components_num = 3u;
            static constexpr size_t locations_num = 1u;
            static_assert(EXTRA <= 1, "too many extra components given");
            using type_name = ct::string_cat<type_prefix<TypeTo>, cts("vec"), ct::string<'3'+EXTRA>>;
      };

      template<typename TypeTo, size_t EXTRA>
      struct type_traits<TypeTo, EXTRA, glm::tvec4>
      {
            static constexpr size_t components_num = 4u;
            static constexpr size_t locations_num = 1u;
            static_assert(EXTRA == 0, "vec4 can't have extra components");
            using type_name = ct::string_cat<type_prefix<TypeTo>, cts("vec4")>;
      };

      template<typename TypeTo, size_t EXTRA>
      struct type_traits<TypeTo, EXTRA, glm::tmat4x4>
      {
            static constexpr size_t components_num = 4u;
            static constexpr size_t locations_num = 4u;
            static_assert(0==EXTRA, "extra components doesn't make sense for matrices");
            using type_name = ct::string_cat<type_prefix<TypeTo>, cts("mat4")>;
      };

      /// @brief shortcut
      template<typename TypeTo, size_t EXTRA, template<typename, glm::precision> class THolder>
      using type_name = typename type_traits<TypeTo, EXTRA, THolder>::type_name;

      /// @brief converter which does nothing if cpu type and glsl type exactly
      /// match and converts data if they differ
      template<typename TGLSLData>
      struct converter
      {
            /// @brief converter for simple type, e.g. int->float or ivec2->vec2
            template<typename TData>
            static inline TGLSLData convert(const TData& data)
            {
               return data;
            }

            /// @brief converter for arrays types, e.g. int[2]->float[2]
            template<typename TData, size_t N>
            static inline TGLSLData convert(const std::array<TData, N>& data)
            {
               TGLSLData glslData;
               for (size_t i = 0; i < N; ++i)
                  glslData[i] = data[i];
               return glslData;
            }

            /// @brief does nothing return same reference, this method should be
            /// optimized out
            static inline const TGLSLData& convert(const TGLSLData& data)
            {
               return data;
            }
      };

      /// @brief traits class for input variable to shader attribute/uniform
      /// @tparam THolder glm template holding values, like glm::tvec3 or glm::tmat4
      /// @tparam TypeFrom type on cpu side
      /// @tparam TypeTo cpp type equivalent to glsl type which should be used on gpu side
      /// @tparam isUniform if true this is a uniform, otherwise it is an attribute
      /// @tparam EXTRA number of extra components on gpu side, only valid for
      /// glm::tvecx types, for example tvec3 declared with EXTRA=1 will result in
      /// vec4 type on gpu side
      /// @tparam N if N is 1 this is traits class for single value, if N > 1 this
      /// is array, e.g. vec3 varname[2]
      template<template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo, bool IsUniform, size_t EXTRA, size_t N>
      struct input_var_traits : std::array<TypeFrom, N>
      {
            static_assert(N >= 1, "glsl type size should be at least 1");

            /// @brief true if this type is attribute
            static constexpr bool is_attribute = !IsUniform;

            /// @brief basic type,  e.g. glm::vec2, or float for glm::tvec1
            template<typename T>
            using basic_type = typename std::conditional<std::is_same<THolder<T, glm::defaultp>,
                                                                      glm::tvec1<T, glm::defaultp>>::value,
                                                         T,
                                                         THolder<T, glm::defaultp>>::type;

            /// @brief data type, basic type or array of basic types
            using data = typename std::conditional<1==N, basic_type<TypeFrom>, std::array<basic_type<TypeFrom>, N>>::type;

            /// @brief data type, basic type or array of basic types
            using glsl_data = typename std::conditional<1==N, basic_type<TypeTo>, std::array<basic_type<TypeTo>, N>>::type;

            /// @brief variable size
            static constexpr size_t size = N;

            /// @brief ctstring containing glsl declaration of variable
            template<typename TName>
            using declaration = ct::string_cat<typename std::conditional<IsUniform, cts("uniform "), cts("in ")>::type,
                                               type_name<TypeTo, EXTRA, THolder>, cts(" "),
                                               TName,
                                               typename std::conditional<1==size, cts(""),
                                                                         ct::string_cat<cts("["), ct::string_from<size_t, size>, cts("]")>>::type,
                                               cts(";\n")>;

            /// @brief attach for attributes
            template<typename TDummy = int> // to enable sfinae
            static void attach(GLint location, const data* ptr = nullptr, typename std::enable_if<is_attribute, TDummy>::type dummy = 0)
            {
               constexpr size_t locations_num  = type_traits<TypeTo, EXTRA, THolder>::locations_num;
               constexpr size_t components_num = type_traits<TypeTo, EXTRA, THolder>::components_num;
               for (size_t n = 0; n < N; ++n)
               {
                  for (size_t i = 0; i < locations_num; ++i)
                  {
                     const size_t locationOffset = i + n*locations_num;
                     glcxx_gl(glEnableVertexAttribArray, location + locationOffset);
                     glcxx_gl(glVertexAttribPointer,
                              location + locationOffset,
                              components_num,
                              type_id<TypeFrom>::value,
                              GL_FALSE, // not normalized
                              sizeof(data),
                              (void*)((const char*)ptr + locationOffset*components_num*sizeof(TypeFrom)));
                  }
               }
            }

            /// @brief detach for attributes
            template<typename TDummy = int> // to enable sfinae
            static void detach(GLint location, typename std::enable_if<is_attribute, TDummy>::type dummy = 0)
            {
               constexpr size_t locations_num  = type_traits<TypeTo, EXTRA, THolder>::locations_num;
               for (size_t n = 0; n < N; ++n)
               {
                  for (size_t i = 0; i < locations_num; ++i)
                  {
                     const size_t locationOffset = i + n*locations_num;
                     glcxx_gl(glDisableVertexAttribArray, location + locationOffset);
                  }
               }
            }
      };
   }

   /// @brief shortcuts
   template<template<typename, glm::precision> class THolder, typename TypeFrom = float, typename TypeTo = float, size_t EXTRA = 0>
   using attrib = glsl::input_var_traits<THolder, TypeFrom, TypeTo, false, EXTRA, 1>;

   template<size_t N, template<typename, glm::precision> class THolder, typename TypeFrom = float, typename TypeTo = float, size_t EXTRA = 0>
   using attrib_arr = glsl::input_var_traits<THolder, TypeFrom, TypeTo, false, EXTRA, N>;

   template<template<typename, glm::precision> class THolder, typename TypeFrom = float, typename TypeTo = float, size_t EXTRA = 0>
   using uniform = glsl::input_var_traits<THolder, TypeFrom, TypeTo, true, EXTRA, 1>;

   template<size_t N, template<typename, glm::precision> class THolder, typename TypeFrom = float, typename TypeTo = float, size_t EXTRA = 0>
   using uniform_arr = glsl::input_var_traits<THolder, TypeFrom, TypeTo, true, EXTRA, N>;
}

#endif
