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

#ifndef GLCXX_UNIFORM_HPP
#define GLCXX_UNIFORM_HPP

#include "glcxx/shader_type.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace glcxx
{
    /// @brief functions to attach uniforms, replace huge amount of different c
    /// functions with properly overloaded fucntions, as their ammount is huge,
    /// use macro to reduce repitition
#define DECLARE_ATTACH_UNIFORM_FUNCTIONS(type, suffix)                  \
    inline void attach_uniform(GLint location, type val) {              \
        glUniform1##suffix(location, val);                              \
    }                                                                   \
    inline void                                                         \
    attach_uniform(GLint location, const glm::tvec1<type>& val) {       \
        glUniform1##suffix(location, val.x);                            \
    }                                                                   \
    inline void                                                         \
    attach_uniform(GLint location, const glm::tvec2<type>& val) {       \
        glUniform2##suffix(location, val.x, val.y);                     \
    }                                                                   \
    inline void                                                         \
    attach_uniform(GLint location, const glm::tvec3<type>& val) {       \
        glUniform3##suffix(location, val.x, val.y, val.z);              \
    }                                                                   \
    inline void                                                         \
    attach_uniform(GLint location, const glm::tvec4<type>& val) {       \
        glUniform4##suffix(location, val.x, val.y, val.z, val.w);       \
    }                                                                   \
    template<size_t N> inline void                                      \
    attach_uniform(GLint location, const std::array<type, N>& val) {    \
        glUniform1##suffix##v(location, N, glm::value_ptr(val[0]));     \
    }                                                                   \
    template<size_t N> inline void                                      \
    attach_uniform(GLint location, const std::array<glm::tvec2<type>, N>& val) { \
        glUniform2##suffix##v(location, N, glm::value_ptr(val[0]));     \
    }                                                                   \
    template<size_t N> inline void                                      \
    attach_uniform(GLint location, const std::array<glm::tvec3<type>, N>& val) { \
        glUniform3##suffix##v(location, glm::value_ptr(val[0]));        \
    }                                                                   \
    template<size_t N> inline void                                      \
    attach_uniform(GLint location, const std::array<glm::tvec4<type>, N>& val) { \
        glUniform4##suffix##v(location, glm::value_ptr(val[0]));        \
    }

    /// @brief actual function definitions
    DECLARE_ATTACH_UNIFORM_FUNCTIONS(float, f);
    DECLARE_ATTACH_UNIFORM_FUNCTIONS(int, i);
    DECLARE_ATTACH_UNIFORM_FUNCTIONS(unsigned int, ui);

    /// @brief overloads for matrices
    inline void attach_uniform(GLint location, const glm::mat2& val) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(val));
    }
    template<size_t N> inline void
    attach_uniform(GLint location, const std::array<glm::mat2, N>& val) {
        glUniformMatrix3fv(location, N, GL_FALSE, glm::value_ptr(val[0]));
    }
    inline void attach_uniform(GLint location, const glm::mat3& val) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(val));
    }
    template<size_t N> inline void
    attach_uniform(GLint location, const std::array<glm::mat3, N>& val) {
        glUniformMatrix3fv(location, N, GL_FALSE, glm::value_ptr(val[0]));
    }
    inline void attach_uniform(GLint location, const glm::mat4& val) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val));
    }
    template<size_t N> inline void
    attach_uniform(GLint location, const std::array<glm::mat4, N>& val) {
        glUniformMatrix4fv(location, N, GL_FALSE, glm::value_ptr(val[0]));
    }

    /// @brief get uniform location
    GLint get_uniform_loc(GLuint program, const char* name);

    /// @brief base implementation for uniform program input
    template<typename ShaderType, typename HostType>
    class uniform_base
    {
        /// @brief location of program input inside program
        GLint _location;

        /// @brief holds actual uniform
        HostType _uniform_data;

    public:
        /// @brief constructor
        uniform_base(GLint location)
            : _location(location)
        {
            attach_uniform(_location, glsl_cast<ShaderType>(_uniform_data));
        }

        /// @brief set uniform as program input
        void set(const HostType& value)
        {
            if (_uniform_data != value)
            {
                _uniform_data = value;
                attach_uniform(_location, glsl_cast<ShaderType>(_uniform_data));
            }
        }

        /// @brief called after program was selected, nothing to do as uniforms
        /// remains attached during program selection change
        void select() const
        {}
    };

    /// @brief holds state of program's uniform
    template<typename Name, typename ShaderType, typename DeclTag = tag::vertex, typename HostType = ShaderType>
    class uniform : public uniform_base<ShaderType, HostType>
    {
        /// @brief base implementation class
        using base = uniform_base<ShaderType, HostType>;

    public:
        /// @brief name
        using name = Name;

        /// @brief declaration tag
        using decl_tag = DeclTag;

        // @brief ctstring containing glsl declaration of variable
        using declaration = ct::string_cat<cts("uniform "), typename glcxx::shader_type::traits<ShaderType>::name, cts(" "), Name, cts(";\n")>;

        /// @brief constructor
        uniform(const GLuint program)
            : base(get_uniform_loc(program, Name::chars))
        {}

        /// @brief named set method
        template<typename InputName>
        std::enable_if_t<std::is_same<InputName, Name>::value>
        set(const HostType& value)
        {
            base::set(value);
        }
    };
}

#endif
