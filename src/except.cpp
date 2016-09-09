/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "glcxx/except.hpp"

glcxx::glprogram_error::glprogram_error(const std::string& msg)
   : _msg(msg)
{}

void glcxx::glprogram_error::prepend(const std::string& msg)
{
   _msg = msg + _msg;
}

void glcxx::glprogram_error::append(const std::string& msg)
{
   _msg += msg;
}

const char* glcxx::glprogram_error::what() const noexcept
{
   return _msg.c_str();
}
