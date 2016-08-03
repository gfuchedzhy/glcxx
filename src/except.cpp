/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "except.hpp"

glcxx::glprogram_error::glprogram_error(const std::string& msg)
   : mMsg(msg)
{}

void glcxx::glprogram_error::prepend(const std::string& msg)
{
   mMsg = msg + mMsg;
}

void glcxx::glprogram_error::append(const std::string& msg)
{
   mMsg += msg;
}

const char* glcxx::glprogram_error::what() const noexcept
{
   return mMsg.c_str();
}
