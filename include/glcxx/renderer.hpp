// Copyright 2015, 2016 Grygoriy Fuchedzhy
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

#ifndef GLCXX_RENDERER_HPP
#define GLCXX_RENDERER_HPP

#include "glcxx/program.hpp"

namespace glcxx
{
    /// @brief returns program src by name, all shaders of any program use same
    /// src as their base program, implementation differences between derived
    /// shaders should be implemented using preprocessor, so client should
    /// provide specializations of this function for all base programs
    template<typename Name>
    const char* get_program_src();

    /// @brief renderer, implements program creation and program selection
    /// @tparam std::pairs of compile time programs
    template<typename... NamedPrograms> class renderer;

    template<typename... Name, typename... Program>
    class renderer<std::pair<Name, Program>...>
    {
        /// @brief disabled stuff
        renderer(const renderer&) = delete;
        renderer& operator=(const renderer& other) = delete;

        /// @brief base program name, e.g. shaded_tex_nmap -> shaded
        template<typename ProgramName>
        using base_name = ct::string_sub<ProgramName, 0, ct::string_find<ProgramName, cts("_")>::value>;

    public:
        /// @brief initializes all programs of this renderer
        renderer(const std::string& glsl_version, const std::string& common_decl)
            : _programs(std::make_unique<Program>(Name::chars, glsl_version, common_decl + get_program_src<base_name<Name>>())...)
        {}

        /// @brief searches given program by name in compile time, selects
        /// requested one and returns it
        template<typename ProgramName>
        auto& get_program();

    private:
        /// @brief program list, @todo unique_ptr can be removed without providing
        /// movability or copyability to programs, when emplace style tuple
        /// constructor would be available
        std::tuple<std::unique_ptr<Program>...> _programs;

        /// @brief current program index, points after last program by default
        size_t _current_program_index = sizeof...(Program);
    };
}

template<typename... Name, typename... Program>
template<typename ProgramName>
auto& glcxx::renderer<std::pair<Name, Program>...>::get_program()
{
    constexpr size_t index = ct::tuple_find<std::tuple<Name...>, ProgramName>::value;
    static_assert(sizeof...(Program) != index, "program name not found");
    auto p = std::get<index>(_programs).get();
    if (index != _current_program_index)
    {
        _current_program_index = index;
        p->select();
    }
    return *p;
}

#endif
