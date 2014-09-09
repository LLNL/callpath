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
