/////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010, Lawrence Livermore National Security, LLC.  
// Produced at the Lawrence Livermore National Laboratory  
// Written by Todd Gamblin, tgamblin@llnl.gov.
// LLNL-CODE-417602
// All rights reserved.  
// 
// This file is part of Libra. For details, see http://github.com/tgamblin/libra.
// Please also read the LICENSE file for further information.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
//  * Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the disclaimer below.
//  * Redistributions in binary form must reproduce the above copyright notice, this list of
//    conditions and the disclaimer (as noted below) in the documentation and/or other materials
//    provided with the distribution.
//  * Neither the name of the LLNS/LLNL nor the names of its contributors may be used to endorse
//    or promote products derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// LAWRENCE LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/////////////////////////////////////////////////////////////////////////////////////////////////
#include "CallpathRuntime.h"

#include <string>
#include <iostream>
using namespace std;

#include "frame.h"
#include "walker.h"
#include "swk_errors.h"
using namespace Dyninst;
using namespace Dyninst::Stackwalker;

#include "FrameId.h"

#ifdef CALLPATH_HAVE_SYMTAB
#include "Symtab.h"
#include "Symbol.h"
#include "AddrLookup.h"
using namespace Dyninst::SymtabAPI;
#endif // CALLPATH_HAVE_SYMTAB

CallpathRuntime::CallpathRuntime()
  : walker(Walker::newWalker()), 
    num_walks(0), 
    bad_walks(0),
    chop_libc_calls(false),
    libc_start_main_addr(0)
{ }


Callpath CallpathRuntime::doStackwalk(size_t wrap_level) {
  num_walks++;  // increment stackwalk counter.

  vector<Frame> swalk;
  bool good = walker->walkStack(swalk);
  if (!good) {
    bad_walks++;
  }

  // chop off wrapping.
  size_t start = (swalk.size() <= wrap_level) ? 0 : wrap_level;

  // build up a temporary callpath
  vector<FrameId> temp;

  for (size_t i=start; i < swalk.size(); i++) {
#ifdef CALLPATH_HAVE_SYMTAB
    if (chop_libc_calls) {
      if (!libc_start_main_addr) {
        string tmp_name;
        swalk[i].getName(tmp_name);
        if (tmp_name == "__libc_start_main") {
          libc_start_main_addr = swalk[i].getRA();
        }
      }        

      if (libc_start_main_addr == swalk[i].getRA()) {
        break;
      }
    }
#endif // CALLPATH_HAVE_SYMTAB

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


void CallpathRuntime::set_chop_libc(bool chop) {
  chop_libc_calls = chop;
#ifndef CALLPATH_HAVE_SYMTAB
  if (chop_libc_calls) {
    cerr << "WARNING: chop_libc_calls is not supported without SymtabAPI.  Paths will not be trimmed." << endl;
  }
#endif // CALLPATH_HAVE_SYMTAB
}

size_t CallpathRuntime::numWalks() {
  return num_walks;
}
  
  
size_t CallpathRuntime::badWalks() {
  return bad_walks;
}
