#ifndef CALLPATH_RUNTIME_H
#define CALLPATH_RUNTIME_H

#include <vector>
#include <stdint.h>
#include "Callpath.h"

namespace Dyninst {
  namespace Stackwalker {
    class Walker;
  }
}

/// This class contains runtime support methods for Callpaths.
/// It's the interface between our modules and DynStackwalker.
/// Each of these contains a Walker for the process that 
/// instantiated it.
class CallpathRuntime {
public:
  /// Default constructor.
  CallpathRuntime();

  /// Returns a newly-traced callpath using this runtime's walker.
  Callpath doStackwalk(size_t wrap_level = 0);
    
  /// Total number of stackwalks done so far.
  size_t numWalks();

  /// Number of bad walks out of total.
  size_t badWalks();

  /// Whether or not to chop off calls above __libc_start_main 
  /// when walking the stack.
  void set_chop_libc(bool chop);

private:
  /// Used by doStackwalk
  Dyninst::Stackwalker::Walker *walker;

  // These counters keep track of stats on how many
  // bad stackwalks we're getting.
  size_t num_walks;  /// total number of stackwalks.
  size_t bad_walks;  /// number of bad stackwalks.
  
  // Keep track of address of __libc_start_main
  bool chop_libc_calls;
  uintptr_t libc_start_main_addr;
};

#endif //CALLPATH_RUNTIME_H
