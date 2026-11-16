#!/bin/sh
# Host test for scripts/write-part.sh: the Part file GFOpenUpTool reads.
# Copyright (c) 2026 AmiCradle Team. GNU LGPL 2.1 or later, like 7-Zip.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
mkdir -p "$T/C" "$T/Docs/7-Zip"
: > "$T/C/7zz"; : > "$T/Docs/7-Zip/License.txt"
sh "$HERE/../scripts/write-part.sh" "$T" 26.06
fail=0
expect() { grep -qxF "$1" "$T/Part" || { echo "FAIL: no line: $1"; fail=1; }; }
expect 'part 7Zip 26.06'
expect 'needs 68040'
expect 'copy "C/7zz" "SYS:C/7zz"'
expect 'copy "Docs/7-Zip/License.txt" "SYS:Docs/7-Zip/License.txt"'
grep -q 'needs part' "$T/Part" && { echo "FAIL: 7zz needs no other part"; fail=1; }
[ "$(grep -c '^copy' "$T/Part")" = 2 ] || { echo "FAIL: copy count (the Part file itself must not be listed)"; fail=1; }
sh "$HERE/../scripts/write-part.sh" "$T" 26.06
[ "$(grep -c '^copy' "$T/Part")" = 2 ] || { echo "FAIL: second run lists the Part file"; fail=1; }
[ $fail = 0 ] && echo "Part file: all checks passed"
exit $fail
