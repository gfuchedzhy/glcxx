// Copyright 2016 Grygorii Fuchedzhy <gfuchedzhy@gmail.com>
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

#ifndef GLCXX_EXCEPT_HPP
#define GLCXX_EXCEPT_HPP

#include <stdexcept>
#include <string>

namespace glcxx
{
    class glprogram_error : public std::exception
    {
        std::string _msg;

    public:
        explicit glprogram_error(const std::string& msg);

        /// @brief modify message
        void prepend(const std::string& msg);
        void append(const std::string& msg);

        const char* what() const noexcept override;
    };

    struct shader_compilation_error : glprogram_error
    {
        using glprogram_error::glprogram_error;
    };

    struct shader_linking_error : glprogram_error
    {
        using glprogram_error::glprogram_error;
    };

    /// @brief thrown when program cannot find location of uniform or attribute
    struct input_location_error : glprogram_error
    {
        using glprogram_error::glprogram_error;
    };
}

#endif
