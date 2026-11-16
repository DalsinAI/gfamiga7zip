/* amiga_wide.c - GFAmiga7Zip: Latin-1 wide strings for libnix.
 * Copyright (c) 2026 AmiCradle Team.  LGPL-2.1+ like 7-Zip. */

#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

/* Wide strings.  The Amiga's own text is Latin-1, so a multibyte string
 * here is one byte per character and every byte maps straight to the
 * same Unicode code point.  libnix's wide-character routines do not agree
 * with this compiler's wchar_t (7zz printed mangled file names and could
 * not find "7z" or "zip" by name), so these replace them. */
size_t wcslen(const wchar_t *s)
{
    const wchar_t *p = s;
    while (*p)
        p++;
    return (size_t)(p - s);
}

int wcscmp(const wchar_t *a, const wchar_t *b)
{
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (*a < *b) ? -1 : (*a > *b);
}

wint_t towupper(wint_t c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 0xE0 && c <= 0xFE && c != 0xF7))
        return c - 0x20;
    return c;
}

wint_t towlower(wint_t c)
{
    if ((c >= 'A' && c <= 'Z') || (c >= 0xC0 && c <= 0xDE && c != 0xD7))
        return c + 0x20;
    return c;
}

size_t mbstowcs(wchar_t *d, const char *s, size_t n)
{
    size_t i = 0;
    if (!d) {
        while (s[i])
            i++;
        return i;
    }
    while (i < n) {
        d[i] = (wchar_t)(unsigned char)s[i];
        if (!s[i])
            return i;
        i++;
    }
    return i;
}

size_t wcstombs(char *d, const wchar_t *s, size_t n)
{
    size_t i = 0;
    for (; d == NULL || i < n; i++) {
        wchar_t c = s[i];
        if ((unsigned long)c > 0xFF)
            return (size_t)-1; /* no Latin-1 form */
        if (d)
            d[i] = (char)c;
        if (!c)
            return i;
    }
    return i;
}
