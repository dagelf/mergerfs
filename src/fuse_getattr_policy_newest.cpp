/*
  ISC License

  Copyright (c) 2022, Antonio SJ Musumeci <trapexit@spawn.link>

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

#include "fuse_getattr_policy_newest.hpp"

#include <limits.h>


FUSE::GETATTR::POLICY::Newest::Newest(const toml::value &toml_)
  : _branches(toml_)
{
  Stat::Func statfunc = Stat::no_follow;

  _statfunc = toml::find_or(toml_,"func","getattr","follow-symlinks",statfunc);
}

int
FUSE::GETATTR::POLICY::Newest::operator()(const gfs::path &fusepath_,
                                          struct stat     *st_,
                                          fuse_timeouts_t *timeout_)
{
  int rv;
  struct stat st;
  gfs::path fullpath;

  st_->st_mtime = std::numeric_limits<time_t>::min();
  for(const auto &branch_group : _branches)
    {
      for(const auto &branch : branch_group)
        {
          fullpath  = branch.path / fusepath_;

          rv = _statfunc(fullpath,&st);
          if(rv != 0)
            continue;
          if(st_->st_mtime < st.st_mtime)
            continue;

          *st_ = st;
        }
    }

  if(st_->st_mtime == std::numeric_limits<time_t>::min())
    return -ENOENT;

  return 0;
}