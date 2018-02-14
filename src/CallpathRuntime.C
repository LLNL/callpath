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
#include "CallpathRuntime.h"

#include "unistd.h"
#include <string>
#include <cstring>
#include "FrameId.h"

#ifdef CALLPATH_USE_DYNINST
#include "frame.h"
#include "walker.h"
#include "swk_errors.h"
using namespace Dyninst;
using namespace Dyninst::Stackwalker;

#elif defined(CALLPATH_USE_BACKTRACE)
#include <execinfo.h>
#include <cstring>
#include "link_utils.h"
#endif // TYPE OF WALKER

using namespace std;


static const char *get_exe_name() {
  static const char *exe_name = NULL;
  if (!exe_name) {
    const size_t bufsize = 4096;
    char buf[bufsize];

    // Try to read procfs for Linux, FreeBSD, and Solaris
    ssize_t sz;
    if ((sz = readlink("/proc/self/exe", buf, bufsize))        != -1 ||
        (sz = readlink("/proc/curproc/file", buf, bufsize))    != -1 ||
        (sz = readlink("/proc/self/path/a.out", buf, bufsize)) != -1) {
      buf[sz] = '\0';
      exe_name = strdup(buf);
    } else {
      exe_name = "";
    }
  }

  return exe_name;
}


CallpathRuntime::CallpathRuntime()
  : walker(NULL),
    num_walks(0),
    bad_walks(0),
    chop_libc_calls(false),
    libc_start_main_addr(0),
    checked_for_libc_start_main(false)
{
#ifdef CALLPATH_USE_DYNINST
  walker = Walker::newWalker();
#endif //CALLPATH_USE_DYNINST
}


CallpathRuntime::~CallpathRuntime() {
#ifdef CALLPATH_USE_DYNINST
  if (walker)
    delete walker;
#endif // CALLPATH_USE_DYNINST
}


void CallpathRuntime::set_chop_libc(bool chop) {
  chop_libc_calls = chop;
}


size_t CallpathRuntime::numWalks() {
  return num_walks;
}


size_t CallpathRuntime::badWalks() {
  return bad_walks;
}


//
// We can use many different tools to walk the stack.  The #ifdef'd
// sections below describe how to do stackwalks with dyninst and
// GNU backtrace.
//
// TODO: libunwind
//
#ifdef CALLPATH_USE_DYNINST

Callpath CallpathRuntime::doStackwalk(size_t wrap_level) {
  num_walks++;  // increment stackwalk counter.

  vector<Frame> swalk;
  bool good = walker->walkStack(swalk);
  if (!good) {
    bad_walks++;
  }

  // chop off wrapping.
  size_t start = (swalk.size() <= wrap_level) ? 0 : wrap_level;

  // check for libc_start_main
  if (chop_libc_calls && !checked_for_libc_start_main) {
    for (size_t i=start; i < swalk.size(); i++) {
      string tmp_name;
      swalk[i].getName(tmp_name);
      if (tmp_name == "__libc_start_main") {
        libc_start_main_addr = swalk[i].getRA();
      }
    }
    // even if we didn't find it, mark this and don't check again.
    checked_for_libc_start_main = true;
  }

  // build up a temporary callpath
  vector<FrameId> temp;
  for (size_t i=start;
       i < swalk.size() && libc_start_main_addr != swalk[i].getRA();
       i++) {
    Dyninst::Offset offset;
    string modname;
    void *symtab;

    if (!swalk[i].getLibOffset(modname, offset, symtab)) {
      temp.push_back(FrameId(ModuleId(), swalk[i].getRA()));
    } else {
      temp.push_back(FrameId(modname, offset));
    }
  }

  return Callpath::create(temp);
}

#else // USE GNU BACKTRACE

Callpath CallpathRuntime::doStackwalk(size_t wrap_level) {
  static const size_t MAX_FRAMES = 64;

  num_walks++;  // increment stackwalk counter.

  // do the stacktrace
  void *swalk[MAX_FRAMES];
  size_t frames = backtrace(swalk, MAX_FRAMES);

  // chop off wrapping.
  size_t start = (frames <= wrap_level) ? 0 : wrap_level;

  // check for libc_start_main return address and record it
  // if it is there.
  if (chop_libc_calls && !checked_for_libc_start_main) {
    char **syms = backtrace_symbols(swalk, frames);
    for (size_t i=start; i < frames; i++) {
      if (strstr("__libc_start_main", syms[i])) {
        libc_start_main_addr = (uintptr_t)swalk[i];
      }
    }
    free(syms);
    checked_for_libc_start_main = true;
  }

  // now build a vector of frameids
  vector<FrameId> temp;
  for (size_t i=start;
       i < frames && (uintptr_t)swalk[i] != libc_start_main_addr;
       i++) {

    const link_map *module = get_module_for_address(swalk[i]);

    if (!module) {
      FrameId frame(ModuleId(), (uintptr_t)swalk[i]);
      temp.push_back(frame);

    } else {
      uintptr_t offset = (uintptr_t)swalk[i];
      offset -= (uintptr_t)module->l_addr;

      const char *modname = module->l_name;
      if (strcmp(modname, "") == 0) {
        modname = get_exe_name();
      }

      FrameId frame(modname, offset);
      temp.push_back(frame);
    }
  }

  // return a new callpath
  return Callpath::create(temp);
}


#endif // type of stackwalks
