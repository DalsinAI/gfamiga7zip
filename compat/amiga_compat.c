/* What libnix leaves out, for 7-Zip's 7zz, and its stack and version string.
 * The file calls 7-Zip makes on an open handle (fchmod, futimens) answer
 * ENOSYS: 7-Zip then sets the protection bits and dates by name once the file
 * is closed (chmod, utimensat), which AmigaDOS does with SetProtection and
 * SetFileDate through libnix.
 * Copyright (c) 2026 AmiCradle Team. GNU LGPL 2.1 or later, like 7-Zip. */
#include <errno.h>
#include <wchar.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>
#include <proto/exec.h>
#include "amiga_compat.h"

/* a Shell gives 4 KB of stack; libnix's startup sets this much up instead */
unsigned long __stack = 256 * 1024;
static const char version[] __attribute__((used)) = "$VER: 7zz 26.06 Amiga (16.11.2026)";

int fchmod(int fd, mode_t mode)
{
    (void)fd; (void)mode;
    errno = ENOSYS;
    return -1;
}

int futimens(int fd, const struct timespec times[2])
{
    (void)fd; (void)times;
    errno = ENOSYS;
    return -1;
}

/* by name: libnix's utimes (SetFileDate); UTIME_OMIT/UTIME_NOW as POSIX says */
int utimensat(int dirfd, const char *path, const struct timespec times[2], int flags)
{
    struct timeval tv[2];
    struct stat st;
    int i;
    (void)dirfd; (void)flags;
    if (!times)
        return utimes(path, NULL);
    if (stat(path, &st) != 0)
        return -1;
    for (i = 0; i < 2; i++) {
        if (times[i].tv_nsec == UTIME_OMIT) {
            tv[i].tv_sec = i ? st.st_mtime : st.st_atime;
            tv[i].tv_usec = 0;
        } else if (times[i].tv_nsec == UTIME_NOW) {
            gettimeofday(&tv[i], NULL);
        } else {
            tv[i].tv_sec = times[i].tv_sec;
            tv[i].tv_usec = times[i].tv_nsec / 1000;
        }
    }
    return utimes(path, tv);
}

/* no user database: 7zz stores and shows numbers, not names (the NDK's
   pwd.h and grp.h give these structures; 7-Zip sees compat/include's) */
struct passwd *getpwuid(uid_t uid) { (void)uid; return NULL; }
struct group *getgrgid(gid_t gid) { (void)gid; return NULL; }

/* AmigaDOS files have no owner to change */
int lchown(const char *path, uid_t owner, gid_t group)
{
    (void)path; (void)owner; (void)group;
    errno = ENOSYS;
    return -1;
}

/* newlib's wide-character string search, which libnix leaves out */
wchar_t *wcsstr(const wchar_t *hay, const wchar_t *needle)
{
    size_t n;
    if (!*needle)
        return (wchar_t *)hay;
    for (n = 0; needle[n]; n++)
        ;
    for (; *hay; hay++) {
        size_t i = 0;
        while (i < n && hay[i] == needle[i])
            i++;
        if (i == n)
            return (wchar_t *)hay;
    }
    return NULL;
}

/* GCC's __sync builtins for 4 bytes (7-Zip's InterlockedIncrement and
   InterlockedDecrement in Threads.c), which the 68040 stove leaves as calls:
   one CPU, so the add with interrupts off is atomic. Named by asm label,
   as GCC keeps the C names for its builtins. */
unsigned int amiga_sync_add_and_fetch_4(volatile void *p, unsigned int v)
    __asm__("___sync_add_and_fetch_4");
unsigned int amiga_sync_sub_and_fetch_4(volatile void *p, unsigned int v)
    __asm__("___sync_sub_and_fetch_4");

unsigned int amiga_sync_add_and_fetch_4(volatile void *p, unsigned int v)
{
    unsigned int r;
    Disable();
    r = *(volatile unsigned int *)p += v;
    Enable();
    return r;
}

unsigned int amiga_sync_sub_and_fetch_4(volatile void *p, unsigned int v)
{
    unsigned int r;
    Disable();
    r = *(volatile unsigned int *)p -= v;
    Enable();
    return r;
}

/* System.cpp's GetRamSize: the Amiga headers clash with 7-Zip's own types */
unsigned long amiga_total_mem(void)
{
    return AvailMem(MEMF_TOTAL);
}

