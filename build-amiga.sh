#!/bin/sh
# Builds 7-Zip's 7zz (the full archiver as a shell command) for GFOS (AmigaOS
# 3.2.3 with GFOpenUp) with a stove: 68040, single-threaded (ST_MODE), with
# 7-Zip's own makefiles (CPP/7zip/Bundles/Alone2/makefile.gcc).
#   STOVE=.../stoves/os32-gcc16-open6 OUT=out ./build-amiga.sh
# 7-Zip is fetched at the commit 7zip.lock gives and patched with patches/;
# nothing third-party is kept here. The build tree is kept, so a run that stops
# at BUDGET_TOTAL seconds goes on next time.
# OUT gets Parts/7Zip: a GFOpenUp part (its Part file, C/7zz and the licence
# and readme under Docs/7-Zip), installed with
#   GFOpenUpTool INSTALL Parts/7Zip
# Copyright (c) 2026 AmiCradle Team. GNU LGPL 2.1 or later, like 7-Zip.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
. "$HERE/7zip.lock"
STOVE=${STOVE:-"$HOME/AmiCradle/stoves/os32-gcc16-open6"}
# outside the checkout: Cradle cleans it (git clean -ffdx) before every job
WORK="${WORK:-$HOME/.cache/gfamiga7zip/build}"; OUT="${OUT:-$WORK/out}"; JOBS=${JOBS:-12}
TC="$STOVE/prefix"; PATH="$TC/bin:$PATH"; export PATH
mkdir -p "$WORK" "$OUT"; OUT=$(cd "$OUT" && pwd)
t0=$(date +%s)

# --- 7-Zip at its pinned commit, with patches/ ---------------------------------
SRC="$WORK/7zip"
PSUM=$( (cat "$HERE"/patches/*.patch; cat "$HERE"/compat/*) | sha256sum | cut -c1-16)
if [ "$(cat "$SRC/.patched" 2>/dev/null)" != "$SZ_COMMIT-$PSUM" ]; then
    rm -rf "$SRC"; mkdir -p "$SRC"
    git -C "$SRC" init -q
    git -C "$SRC" fetch -q --depth 1 "$SZ_GIT" "$SZ_COMMIT"
    git -C "$SRC" checkout -q FETCH_HEAD
    [ "$(git -C "$SRC" rev-parse HEAD)" = "$SZ_COMMIT" ] || { echo "commit mismatch"; exit 1; }
    for p in "$HERE"/patches/*.patch; do
        patch -s -d "$SRC" -p1 < "$p" || { echo "patch failed: $p"; exit 1; }
    done
    echo "$SZ_COMMIT-$PSUM" > "$SRC/.patched"
fi

# --- compat: what libnix leaves out ---------------------------------------------
ARCH="-noixemul -m68040 -fno-delete-null-pointer-checks -D__amigaos__ -include $HERE/compat/amiga_compat.h"
m68k-amigaos-gcc $ARCH -O2 -c "$HERE/compat/amiga_compat.c" -o "$WORK/amiga_compat.o"
# 7-Zip's own sources see compat/include's pwd.h and grp.h, not the NDK's
ARCH="$ARCH -I$HERE/compat/include"

# --- 7zz with upstream's makefiles ------------------------------------------------
# CFLAGS_BASE without -Werror and -fPIC (no shared objects on the Amiga);
# LIB2 without -lpthread -ldl (ST_MODE: no threads, no plugins)
B="$SRC/CPP/7zip/Bundles/Alone2"
left=$(( ${BUDGET_TOTAL:-840} - ($(date +%s) - t0) ))
set +e
(cd "$B" && timeout "$left" nice -n 19 make -k -j"$JOBS" -f ../../cmpl_gcc.mak \
    CROSS_COMPILE=m68k-amigaos- ST_MODE=1 O=b/amiga MY_ARCH="$ARCH" \
    CFLAGS_BASE="-O2 -c -DNDEBUG -D_REENTRANT" CFLAGS_WARN_WALL= LFLAGS_STRIP= \
    LFLAGS_NOEXECSTACK= CC_SHARED= LIB2="$WORK/amiga_compat.o" > "$WORK/make.log" 2>&1)
rc=$?
set -e
grep -E -B2 'error|Error|undefined reference' "$WORK/make.log" | head -80
[ "$rc" = 0 ] || { [ "$rc" = 124 ] && echo "out of time: run again to go on"; tail -20 "$WORK/make.log"; echo "build: $(( $(date +%s) - t0 )) s"; exit "$rc"; }
echo "build: $(( $(date +%s) - t0 )) s"

# --- the GFOpenUp part: Parts/7Zip ------------------------------------------------
P="$OUT/Parts/7Zip"; rm -rf "$P"; mkdir -p "$P/C" "$P/Docs/7-Zip"
m68k-amigaos-strip -o "$P/C/7zz" "$B/b/amiga/7zz"
cp "$SRC/DOC/License.txt" "$P/Docs/7-Zip/License.txt"
cp "$HERE/README.md" "$P/Docs/7-Zip/ReadMe"
[ -f "$SRC/DOC/7zip.txt" ] && cp "$SRC/DOC/7zip.txt" "$P/Docs/7-Zip/7zip.txt"
sh "$HERE/scripts/write-part.sh" "$P" "$SZ_VERSION"
ls -l "$P/C/7zz"
echo "part: $(grep -c '^copy' "$P/Part") files, $(du -sk "$P" | cut -f1) KB"
echo "total: $(( $(date +%s) - t0 )) s"
