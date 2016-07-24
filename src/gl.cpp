/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "gl.hpp"

glcxx::glerror_cb_t glcxx::detail::glerror_cb;

void glcxx::set_glerror_cb(glerror_cb_t cb)
{
   detail::glerror_cb = std::move(cb);
}
