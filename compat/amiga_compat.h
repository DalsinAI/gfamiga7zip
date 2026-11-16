/* Taken into every 7-Zip source (-include): the Amiga build's settings.
 * Copyright (c) 2026 AmiCradle Team. GNU LGPL 2.1 or later, like 7-Zip. */
#ifndef AMIGA_COMPAT_H
#define AMIGA_COMPAT_H
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#include <sys/stat.h>
#include <fcntl.h>
/* POSIX 2008 names newlib's headers keep behind __ATFILE_VISIBLE; 7-Zip's
   Unix code uses them, compat/amiga_compat.c gives the calls */
#ifndef UTIME_NOW
#define UTIME_NOW  ((1L << 30) - 1L)
#define UTIME_OMIT ((1L << 30) - 2L)
#endif
#ifndef AT_FDCWD
#define AT_FDCWD -2
#endif
#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif
#ifdef __cplusplus
extern "C" {
#endif
int futimens(int fd, const struct timespec times[2]);
int utimensat(int dirfd, const char *path, const struct timespec times[2], int flags);
int fchmod(int fd, mode_t mode);
#ifdef __cplusplus
}
#endif
#endif
