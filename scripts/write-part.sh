#!/bin/sh
# Writes the Part file GFOpenUpTool INSTALL reads (gfopenup src/openuptool.c):
# one "copy" line per file under the part drawer, each to SYS:<its path>.
#   write-part.sh <part drawer> <version>
# Copyright (c) 2026 AmiCradle Team. GNU LGPL 2.1 or later, like 7-Zip.
set -eu
P=$1; V=$2
{
    echo "part 7Zip $V"
    echo "; 7-Zip: the 7zz archiver (7z, zip, tar, xz, gz, bz2, iso, rar and more) as a shell command"
    echo "needs 68040"
    (cd "$P" && find C Docs -type f ! -name Part 2>/dev/null | LC_ALL=C sort) | while read -r f; do
        echo "copy \"$f\" \"SYS:$f\""
    done
} > "$P/Part"
