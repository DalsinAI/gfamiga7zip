/* Host test for compat/amiga_wide.c (built with its names renamed so the
 * host C library's own stay out of the way).
 * Copyright (c) 2026 AmiCradle Team. GNU LGPL 2.1 or later, like 7-Zip. */
#include <stdio.h>
#include <stddef.h>
#include <wchar.h>

size_t aw_wcslen(const wchar_t *);
int aw_wcscmp(const wchar_t *, const wchar_t *);
wint_t aw_towupper(wint_t);
wint_t aw_towlower(wint_t);
size_t aw_mbstowcs(wchar_t *, const char *, size_t);
size_t aw_wcstombs(char *, const wchar_t *, size_t);
wchar_t *aw_wmemcpy(wchar_t *, const wchar_t *, size_t);
wchar_t *aw_wmemmove(wchar_t *, const wchar_t *, size_t);
wchar_t *aw_wmemset(wchar_t *, wchar_t, size_t);
wchar_t *aw_wcscpy(wchar_t *, const wchar_t *);
wchar_t *aw_wcsncpy(wchar_t *, const wchar_t *, size_t);
wchar_t *aw_wcscat(wchar_t *, const wchar_t *);
int aw_wcsncmp(const wchar_t *, const wchar_t *, size_t);
wchar_t *aw_wcschr(const wchar_t *, wchar_t);

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); fails++; } } while (0)

int main(void)
{
    wchar_t w[32];
    char a[32];
    const char *name = "RAM:z/t.7z";
    const char latin[] = { 'S', (char)0xE9, 'a', 0 }; /* "Séa" */

    CHECK(aw_mbstowcs(NULL, name, 0) == 10);
    CHECK(aw_mbstowcs(w, name, 32) == 10);
    CHECK(aw_wcslen(w) == 10 && w[3] == L':' && w[9] == L'z');
    CHECK(aw_wcscmp(w, L"RAM:z/t.7z") == 0);
    CHECK(aw_wcscmp(L"7z", L"zip") < 0 && aw_wcscmp(L"zip", L"7z") > 0);
    CHECK(aw_wcscmp(L"ab", L"abc") < 0);
    CHECK(aw_mbstowcs(w, latin, 32) == 3 && w[1] == 0xE9);
    CHECK(aw_wcstombs(NULL, w, 0) == 3);
    CHECK(aw_wcstombs(a, w, 32) == 3 && (unsigned char)a[1] == 0xE9 && a[3] == 0);
    CHECK(aw_wcstombs(a, L"\x20AC", 32) == (size_t)-1); /* euro: no Latin-1 */
    CHECK(aw_towupper('a') == 'A' && aw_towupper(0xE9) == 0xC9 && aw_towupper(0xF7) == 0xF7);
    CHECK(aw_towlower('Z') == 'z' && aw_towlower(0xC9) == 0xE9 && aw_towlower(0xD7) == 0xD7);
    CHECK(aw_mbstowcs(w, name, 4) == 4); /* stops at n with no terminator */

    /* 7-Zip's UString copies with wmemcpy: a whole 25-character error
     * text must come through, not the first few characters */
    {
        const wchar_t *e = L"No such file or directory";
        wchar_t b[40];
        CHECK(aw_wmemcpy(b, e, 26) == b && aw_wcscmp(b, e) == 0);
        aw_wmemmove(b + 1, b, 25); /* overlapping, forward */
        CHECK(b[0] == L'N' && b[1] == L'N' && b[25] == L'y');
        b[26] = 0;
        aw_wmemmove(b, b + 1, 26); /* overlapping, back */
        CHECK(aw_wcscmp(b, e) == 0);
        aw_wmemset(b, L'x', 3); b[3] = 0;
        CHECK(aw_wcscmp(b, L"xxx") == 0);
        CHECK(aw_wcscpy(b, L"RAM:") == b && aw_wcscat(b, L"z/t.7z") == b);
        CHECK(aw_wcscmp(b, L"RAM:z/t.7z") == 0);
        aw_wcsncpy(b, L"ab", 5);
        CHECK(b[0] == L'a' && b[2] == 0 && b[4] == 0);
        CHECK(aw_wcsncmp(L"7zip", L"7zz", 2) == 0 && aw_wcsncmp(L"7zip", L"7zz", 3) < 0);
        CHECK(aw_wcschr(L"RAM:z", L':') != NULL && *aw_wcschr(L"RAM:z", L':') == L':');
        CHECK(aw_wcschr(L"RAM", L'/') == NULL && aw_wcschr(L"RAM", 0) != NULL);
    }

    printf("wide_test: %s\n", fails ? "FAIL" : "23/23 pass");
    return fails != 0;
}
