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
#ifndef CALLPATH_RUNTIME_H
#define CALLPATH_RUNTIME_H

#include <vector>
#include <stdint.h>
#include "Callpath.h"

namespace Dyninst {
namespace Stackwalker {
class Walker;
}}

/// This class contains runtime support methods for Callpaths.
/// It's the interface between our modules and DynStackwalker.
/// Each of these contains a Walker for the process that
/// instantiated it.
class CallpathRuntime {
public:
  /// Default constructor.
  CallpathRuntime();

  /// Default constructor.
  ~CallpathRuntime();

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
  size_t num_walks;  ///< total number of stackwalks.
  size_t bad_walks;  ///< number of bad stackwalks.

  /// whether to chop calls found below __libc_start_main
  bool chop_libc_calls;

  /// cached address of __libc_start_main
  uintptr_t libc_start_main_addr;
  bool checked_for_libc_start_main;
};

#endif //CALLPATH_RUNTIME_H
