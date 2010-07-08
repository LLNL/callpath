#include "CallpathRuntime.h"

#include <string>
#include <iostream>
using namespace std;

#include "frame.h"
#include "walker.h"
using namespace Dyninst;
using namespace Dyninst::Stackwalker;

#include "FrameId.h"

#ifdef HAVE_SYMTAB
#include "Symtab.h"
#include "Symbol.h"
#include "AddrLookup.h"
using namespace Dyninst::SymtabAPI;
#endif // HAVE_SYMTAB

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
#ifdef HAVE_SYMTAB
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
#endif // HAVE_SYMTAB

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
#ifndef HAVE_SYMTAB
  if (chop_libc_calls) {
    cerr << "WARNING: chop_libc_calls is not supported without SymtabAPI.  Paths will not be trimmed." << endl;
  }
#endif // HAVE_SYMTAB
}

size_t CallpathRuntime::numWalks() {
  return num_walks;
}
  
  
size_t CallpathRuntime::badWalks() {
  return bad_walks;
}
