/* 7-Zip's grp.h on the Amiga: see pwd.h here. getgrgid answers NULL.
 * Copyright (c) 2026 AmiCradle Team. GNU LGPL 2.1 or later, like 7-Zip. */
#ifndef GFAMIGA7ZIP_GRP_H
#define GFAMIGA7ZIP_GRP_H
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif
struct group { char *gr_name; };
struct group *getgrgid(gid_t gid);
#ifdef __cplusplus
}
#endif
#endif
