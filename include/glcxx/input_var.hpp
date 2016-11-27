// Copyright 2015, 2016 Grygorii Fuchedzhy <gfuchedzhy@gmail.com>
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef GLCXX_INPUT_VAR_HPP
#define GLCXX_INPUT_VAR_HPP

#include "glcxx/shader_type.hpp"
#include "glcxx/except.hpp"
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace glcxx
{
    /// @brief get attribute location
    GLint get_attrib_loc(GLuint program, const char* name);

    namespace glsl
    {
        /// @brief traits class for input variable to shader attribute/uniform
        /// @tparam Holder glm template holding values, like glm::tvec3 or glm::tmat4
        /// @tparam TypeFrom type on cpu side
        /// @tparam TypeTo cpp type equivalent to glsl type which should be used on gpu side
        /// @tparam isUniform if true this is a uniform, otherwise it is an attribute
        /// @tparam N if N is 1 this is traits class for single value, if N > 1 this
        /// is array, e.g. vec3 varname[2]
        template<template<typename, glm::precision> class Holder, typename TypeFrom, typename TypeTo, bool IsUniform, size_t N>
        struct input_var_traits : std::array<TypeFrom, N>
        {
            static_assert(N >= 1, "glsl type size should be at least 1");

            /// @brief true if this type is attribute
            static constexpr bool is_attribute = !IsUniform;

            /// @brief basic type,  e.g. glm::vec2, or float for glm::tvec1
            template<typename T>
            using basic_type = typename std::conditional<std::is_same<Holder<T, glm::defaultp>,
                                                                      glm::tvec1<T, glm::defaultp>>::value,
                                                         T,
                                                         Holder<T, glm::defaultp>>::type;

            /// @brief data type, basic type or array of basic types
            using data = typename std::conditional<1==N, basic_type<TypeFrom>, std::array<basic_type<TypeFrom>, N>>::type;

            /// @brief data type, basic type or array of basic types
            using glsl_data = typename std::conditional<1==N, basic_type<TypeTo>, std::array<basic_type<TypeTo>, N>>::type;

            /// @brief variable size
            static constexpr size_t size = N;

            /// @brief ctstring containing glsl declaration of variable
            template<typename Name>
            using declaration = ct::string_cat<typename std::conditional<IsUniform, cts("uniform "), cts("in ")>::type,
                                               shader_type::name<Holder<TypeTo, glm::defaultp>>, cts(" "),
                                               Name,
                                               typename std::conditional<1==size, cts(""),
                                                                         ct::string_cat<cts("["), ct::string_from<size_t, size>, cts("]")>>::type,
                                               cts(";\n")>;

            /// @brief attach for attributes
            template<typename Dummy = int> // to enable sfinae
            static void attach(GLint location, const data* ptr = nullptr, typename std::enable_if<is_attribute, Dummy>::type = 0)
            {
                constexpr size_t locations_num  = shader_type::traits<Holder<TypeTo, glm::defaultp>>::locations_num;
                constexpr size_t components_num = shader_type::traits<Holder<TypeTo, glm::defaultp>>::components_num;
                for (size_t n = 0; n < N; ++n)
                {
                    for (size_t i = 0; i < locations_num; ++i)
                    {
                        const size_t locationOffset = i + n*locations_num;
                        glEnableVertexAttribArray(location + locationOffset);
                        glVertexAttribPointer(location + locationOffset,
                                              components_num,
                                              shader_type::traits<TypeFrom>::id,
                                              GL_FALSE, // not normalized
                                              sizeof(data),
                                              (void*)((const char*)ptr + locationOffset*components_num*sizeof(TypeFrom)));
                    }
                }
            }

            /// @brief detach for attributes
            template<typename Dummy = int> // to enable sfinae
            static void detach(GLint location, typename std::enable_if<is_attribute, Dummy>::type = 0)
            {
                constexpr size_t locations_num  = shader_type::traits<Holder<TypeTo, glm::defaultp>>::locations_num;
                for (size_t n = 0; n < N; ++n)
                    for (size_t i = 0; i < locations_num; ++i)
                        glDisableVertexAttribArray(location + i + n*locations_num);
            }

            /// @brief attach for attributes
            template<typename Dummy = int> // to enable sfinae
            static void divisor(GLint location, GLuint divisor, typename std::enable_if<is_attribute, Dummy>::type = 0)
            {
                constexpr size_t locations_num  = shader_type::traits<Holder<TypeTo, glm::defaultp>>::locations_num;
                for (size_t n = 0; n < N; ++n)
                    for (size_t i = 0; i < locations_num; ++i)
                        glVertexAttribDivisor(location + i + n*locations_num, divisor);
            }
        };
    }

    /// @brief shortcuts
    template<template<typename, glm::precision> class Holder, typename TypeFrom = float, typename TypeTo = float>
    using attrib = glsl::input_var_traits<Holder, TypeFrom, TypeTo, false, 1>;

    template<size_t N, template<typename, glm::precision> class Holder, typename TypeFrom = float, typename TypeTo = float>
    using attrib_arr = glsl::input_var_traits<Holder, TypeFrom, TypeTo, false, N>;
}

#endif
