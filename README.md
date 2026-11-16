# GFAmiga7Zip

7-Zip's `7zz`, the full archiver, as a shell command for GFOS (AmigaOS 3.2.3 with GFOpenUp), installed as a GFOpenUp part.

- Packs and unpacks 7z, zip, tar, xz, gzip, bzip2 and zstd; unpacks rar, iso, cab, lzh, dmg and many more (`7zz i` lists them).
- Needs a 68040; no other part needed.
- Install: `GFOpenUpTool INSTALL Parts/7Zip`. It goes to `SYS:C/7zz`, with the licence and readme in `SYS:Docs/7-Zip`.
- AmigaDOS paths work as usual: `7zz a RAM:backup.7z Work:Projects/`, `7zz x Work:Downloads/game.zip -oWork:Games/`.
- It sets up its own 256 KB stack, so the Shell's stack size does not matter.

Build with a stove: `STOVE=.../os32-gcc16-open6 OUT=out ./build-amiga.sh`. 7-Zip itself is fetched at the commit in `7zip.lock` and patched with `patches/`; nothing third-party is kept here. Host tests: `make -C tests test`.

7-Zip is Copyright (C) Igor Pavlov, GNU LGPL 2.1 or later, with the unRAR licence restriction on its RAR code (see LICENSE); this port is under the same terms.

Designed, tested and delivered by The AmiCradle Team.
