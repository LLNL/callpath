//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010-2014, Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory.
//
// This file is part of the Callpath library.
// Written by Todd Gamblin, tgamblin@llnl.gov, All rights reserved.
// LLNL-CODE-647183
//
// For details, see https://github.com/scalability-llnl/callpath
//
// For details, see https://scalability-llnl.github.io/spack
// Please also see the LICENSE file for our notice and the LGPL.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License (as published by
// the Free Software Foundation) version 2.1 dated February 1999.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the terms and
// conditions of the GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//////////////////////////////////////////////////////////////////////////////
#include <string>
#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <cassert>
#include <vector>

#include "CallpathRuntime.h"

using namespace std;


// A 64-bit return value that can be passed in r3 and r4
#define RVAL 0x1234567890123456ll

// A huge return value that would require more registers
// (make sure our scheme isn't interfering with return value registers)
struct huge_return_value_t {
  int a, b, c, d, e, f, g, h;
  huge_return_value_t(int aa, int bb, int cc, int dd, int ee, int ff, int gg, int hh) 
    : a(aa), b(bb), c(cc), d(dd), e(ee), f(ff), g(gg), h(hh) { }

  bool operator==(const huge_return_value_t& other) {
    return (a == other.a && b == other.b && c == other.c && d == other.d &&
            e == other.e && f == other.f && g == other.g && h == other.h);
  }
};
huge_return_value_t hrv(8, 2, 4, 5, 1, 67, 42, 245);


CallpathRuntime runtime;


void f6() { 
  for (size_t i=0; i < 100; i++) {
    Callpath path = runtime.doStackwalk();
  }
}


int64_t f5() { 
  f6(); 
  return RVAL;
}


int64_t f4() { 
  return f5();
}


huge_return_value_t f3() { 
  for (size_t i=1; i < 10; i++) {
    f4(); 
  }
  f5();
  return hrv;
}


int64_t f2(int a, int b, int c, int d, int e, int f, int g, int h) { 
  assert(a == 8); assert(b == 7); assert(c == 6); assert(d == 5);
  assert(e == 4); assert(f == 3); assert(g == 2); assert(h == 1);

  huge_return_value_t rval = f3();
  assert(rval == hrv);

  rval = f3();
  assert(rval == hrv);

  return RVAL;
}


int64_t f1(int a, int b, int c, int d, int e, int f, int g, int h) {
  assert(a == 1); assert(b == 2); assert(c == 3); assert(d == 4);
  assert(e == 5); assert(f == 6); assert(g == 7); assert(h == 8);
  int64_t r = f2(8, 7, 6, 5, 4, 3, 2, 1);
  assert(r == RVAL);
  return r;
}



int main(int argc, char **argv) {
  runtime.set_chop_libc(true);

  f1(1, 2, 3, 4, 5, 6, 7, 8);

  cout << runtime.numWalks() << " total stackwalks." << endl;
  Callpath::dump(cout);

  exit(0);
}


