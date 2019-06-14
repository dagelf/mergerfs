/*
  ISC License

  Copyright (c) 2019, Antonio SJ Musumeci <trapexit@spawn.link>

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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

namespace fs
{
  static
  inline
  int
  fstatat(const int    dirfd,
          const char  *pathname,
          struct stat *statbuf,
          const int    flags)
  {
    return ::fstatat(dirfd,
                     pathname,
                     statbuf,
                     flags);
  }

  static
  inline
  int
  fstatat_nofollow(const int    dirfd,
                   const char  *pathname,
                   struct stat *statbuf)
  {
    return fs::fstatat(dirfd,
                       pathname,
                       statbuf,
                       AT_SYMLINK_NOFOLLOW);
  }
}
