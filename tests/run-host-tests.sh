#!/bin/sh
# Host tests: 7-Zip's FileName.cpp with patches/ applied, built for the host
# with -D__amigaos__, against tests/filename_test.cpp.
#   sh tests/run-host-tests.sh      (WORK=... to keep the checkout elsewhere)
# Copyright (c) 2026 AmiCradle Team. GNU LGPL 2.1 or later, like 7-Zip.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
. "$HERE/7zip.lock"
WORK=${WORK:-$(mktemp -d)}
SRC="$WORK/7zip"
if [ "$(git -C "$SRC" rev-parse HEAD 2>/dev/null)" != "$SZ_COMMIT" ]; then
    rm -rf "$SRC"; mkdir -p "$SRC"; git -C "$SRC" init -q
    git -C "$SRC" fetch -q --depth 1 "$SZ_GIT" "$SZ_COMMIT"; git -C "$SRC" checkout -q FETCH_HEAD
    for p in "$HERE"/patches/*.patch; do patch -s -d "$SRC" -p1 < "$p"; done
fi
mkdir -p "$SRC/CPP/7zip/AmigaTest"
cp "$HERE/tests/filename_test.cpp" "$SRC/CPP/7zip/AmigaTest/"
C="$SRC/CPP"
cc -c -o "$WORK/Alloc.o" "$SRC/C/Alloc.c"
c++ -std=c++11 -D__amigaos__ -I"$C" -I"$C/7zip/Bundles/Alone2" -o "$WORK/filename_test" \
    "$C/7zip/AmigaTest/filename_test.cpp" "$C/Windows/FileName.cpp" "$C/Common/MyString.cpp" \
    "$C/Common/StringConvert.cpp" "$C/Common/UTFConvert.cpp" "$C/Common/MyVector.cpp" "$C/Common/IntToString.cpp" "$C/Common/MyWindows.cpp" \
    "$C/Windows/FileDir.cpp" "$C/Windows/FileFind.cpp" "$C/Windows/FileIO.cpp" "$C/Windows/TimeUtils.cpp" "$C/Common/Wildcard.cpp" "$WORK/Alloc.o" 2>&1 | grep -v "^$" | head -30
"$WORK/filename_test"

# compat/amiga_wide.c: Latin-1 wide strings in place of libnix's
R="-Dwcslen=aw_wcslen -Dwcscmp=aw_wcscmp -Dtowupper=aw_towupper -Dtowlower=aw_towlower -Dmbstowcs=aw_mbstowcs -Dwcstombs=aw_wcstombs -Dwmemcpy=aw_wmemcpy -Dwmemmove=aw_wmemmove -Dwmemset=aw_wmemset -Dwcscpy=aw_wcscpy -Dwcsncpy=aw_wcsncpy -Dwcscat=aw_wcscat -Dwcsncmp=aw_wcsncmp -Dwcschr=aw_wcschr"
cc $R -c -o "$WORK/amiga_wide.o" "$HERE/compat/amiga_wide.c"
cc -o "$WORK/wide_test" "$HERE/tests/wide_test.c" "$WORK/amiga_wide.o"
"$WORK/wide_test"
