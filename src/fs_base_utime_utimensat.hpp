/*
  ISC License

  Copyright (c) 2016, Antonio SJ Musumeci <trapexit@spawn.link>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef __FS_BASE_UTIME_UTIMENSAT_HPP__
#define __FS_BASE_UTIME_UTIMENSAT_HPP__

#include <string>

#include <fcntl.h>
#include <sys/stat.h>

namespace fs
{
  static
  inline
  int
  utime(const int              dirfd,
        const std::string     &path,
        const struct timespec  times[2],
        const int              flags)
  {
    return ::utimensat(dirfd,path.c_str(),times,flags);
  }

  static
  inline
  int
  utime(const int             fd,
        const struct timespec times[2])
  {
    return ::futimens(fd,times);
  }
}

#endif
