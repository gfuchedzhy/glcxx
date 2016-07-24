/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "gl.hpp"

glerror_cb_t detail::glerror_cb;

void set_glerror_cb(glerror_cb_t cb)
{
   detail::glerror_cb = std::move(cb);
}
