/* 7-Zip's pwd.h on the Amiga: the NDK's brings exec/types.h, whose BYTE, WORD
 * and BOOL clash with 7-Zip's own. There is no user database: getpwuid
 * (compat/amiga_compat.c) answers NULL, and 7zz keeps owner numbers.
 * Copyright (c) 2026 AmiCradle Team. GNU LGPL 2.1 or later, like 7-Zip. */
#ifndef GFAMIGA7ZIP_PWD_H
#define GFAMIGA7ZIP_PWD_H
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif
struct passwd { char *pw_name; };
struct passwd *getpwuid(uid_t uid);
#ifdef __cplusplus
}
#endif
#endif
