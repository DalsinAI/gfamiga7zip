// Host test for patches/0001's AmigaDOS paths in 7-Zip's Windows/FileName.cpp,
// built on the host with -D__amigaos__ (tests/run-host-tests.sh).
// Copyright (c) 2026 AmiCradle Team. GNU LGPL 2.1 or later, like 7-Zip.
#include "StdAfx.h"
#include <stdio.h>
#include <unistd.h>
#include "../../Windows/FileName.h"

// FileName.cpp defines the 8-bit form for non-Windows builds without a header line
namespace NWindows { namespace NFile { namespace NName {
unsigned GetRootPrefixSize(CFSTR s) throw();
}}}

using namespace NWindows::NFile::NName;

static int fails;

static void expect_root(const char *s, unsigned want)
{
  const unsigned got = GetRootPrefixSize(s);
  const UString u = fs2us(FString(s));
  const unsigned gotw = GetRootPrefixSize(u.Ptr());
  if (got != want || gotw != want)
  {
    printf("FAIL: GetRootPrefixSize(\"%s\") = %u/%u, want %u\n", s, got, gotw, want);
    fails++;
  }
  if (IsAbsolutePath(u.Ptr()) != (want != 0))
  {
    printf("FAIL: IsAbsolutePath(\"%s\")\n", s);
    fails++;
  }
}

static void expect_prefix(const char *s, const char *want)
{
  FString f(s);
  NormalizeDirPathPrefix(f);
  if (f != FString(want))
  {
    printf("FAIL: NormalizeDirPathPrefix(\"%s\") = \"%s\", want \"%s\"\n", s, f.Ptr(), want);
    fails++;
  }
}

static void expect_full(const char *dir, const char *s, const char *want)
{
  FString res;
  if (!GetFullPath(FString(dir), FString(s), res) || res != FString(want))
  {
    printf("FAIL: GetFullPath(\"%s\", \"%s\") = \"%s\", want \"%s\"\n", dir, s, res.Ptr(), want);
    fails++;
  }
}

int main()
{
  expect_root("Work:", 5);
  expect_root("RAM:x.7z", 4);
  expect_root("Work:Music/a.7z", 5);
  expect_root("a.7z", 0);
  expect_root("Music/a.7z", 0);
  expect_root("/a.7z", 0);            // "/" is the parent drawer: relative
  expect_root("dir/x:y", 0);          // a colon after a "/" names no volume

  expect_prefix("Work:", "Work:");    // never "Work:/", which is the parent
  expect_prefix("Work:Music", "Work:Music/");
  expect_prefix("Work:Music/", "Work:Music/");

  expect_full("Work:Music/", "a.7z", "Work:Music/a.7z");
  expect_full("Work:", "a.7z", "Work:a.7z");
  expect_full("Work:Music/", "RAM:a.7z", "RAM:a.7z");

  if (!fails)
    printf("AmigaDOS paths in FileName.cpp: all checks passed\n");
  return fails != 0;
}
