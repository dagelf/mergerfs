/*
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

#define _DEFAULT_SOURCE

#include "config.hpp"
#include "dirinfo.hpp"
#include "errno.hpp"
#include "fs_base_closedir.hpp"
#include "fs_base_dirfd.hpp"
#include "fs_base_opendir.hpp"
#include "fs_base_readdir.hpp"
#include "fs_base_fstatat.hpp"
#include "fs_base_stat.hpp"
#include "fs_devid.hpp"
#include "fs_inode.hpp"
#include "fs_path.hpp"
#include "hashset.hpp"
#include "rwlock.hpp"
#include "ugid.hpp"

#include <fuse.h>
#include <fuse_dirents.h>

#include <string>
#include <vector>

#include <dirent.h>

using std::string;
using std::vector;

#define NO_OFFSET 0


static
int
dot_or_dotdot(const char *s_)
{
  return ((s_[0] == '.') &&
          ((s_[1] == '\0') ||
           ((s_[1] == '.') && (s_[2] == '\0'))));
}


namespace l
{
  static
  int
  readdir_plus(const Branches &branches_,
               const char     *dirname_,
               fuse_dirents_t *buf_)
  {
    dev_t dev;
    HashSet names;
    string basepath;
    struct stat st;
    fuse_entry_t entry;

    entry.nodeid           = 0;
    entry.generation       = 0;
    entry.entry_valid      = 1;
    entry.attr_valid       = 1;
    entry.entry_valid_nsec = 0;
    entry.attr_valid_nsec  = 0;
    for(size_t i = 0, ei = branches_.size(); i != ei; i++)
      {
        int rv;
        int dirfd;
        DIR *dh;

        basepath = fs::path::make(&branches_[i].path,dirname_);

        dh = fs::opendir(basepath);
        if(!dh)
          continue;

        dirfd = fs::dirfd(dh);
        dev   = fs::devid(dirfd);
        if(dev == (dev_t)-1)
          dev = i;

        rv = 0;
        for(struct dirent *de = fs::readdir(dh); de && !rv; de = fs::readdir(dh))
          {
            rv = names.put(de->d_name,_D_EXACT_NAMLEN(de));
            if(rv == 0)
              continue;

            rv = fs::fstatat_nofollow(dirfd,de->d_name,&st);
            if(rv == -1)
              memset(&st,0,sizeof(st));

            de->d_ino = fs::inode::recompute(de->d_ino,dev);
            st.st_ino = fs::inode::recompute(st.st_ino,dev);

            rv = fuse_dirents_add_plus(buf_,de,&entry,&st);
            if(rv)
              return (fs::closedir(dh),-ENOMEM);
          }

        fs::closedir(dh);
      }

    return 0;
  }
}

namespace FUSE
{
  int
  readdir_plus(fuse_file_info *ffi_,
               fuse_dirents_t *buf_)
  {
    DirInfo                 *di     = reinterpret_cast<DirInfo*>(ffi_->fh);
    const fuse_context      *fc     = fuse_get_context();
    const Config            &config = Config::get(fc);
    const ugid::Set          ugid(fc->uid,fc->gid);
    const rwlock::ReadGuard  readlock(&config.branches_lock);

    return l::readdir_plus(config.branches,
                           di->fusepath.c_str(),
                           buf_);
  }
}
