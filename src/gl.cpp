/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "gl.hpp"

namespace glcxx
{
   namespace detail
   {
      glerror_cb_t glerror_cb;
   }

   void set_glerror_cb(glerror_cb_t cb)
   {
      detail::glerror_cb = std::move(cb);
   }
}
