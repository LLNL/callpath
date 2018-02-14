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
#ifndef FRAME_ID_H
#define FRAME_ID_H

#include <stdint.h>
#include <string>
#include <set>
#include <iostream>

#include "callpath-config.h"
#ifdef CALLPATH_HAVE_MPI
#include <mpi.h>
#endif // CALLPATH_HAVE_MPI

#include "ModuleId.h"

///
/// Module, offset representation for Callpath frames.
/// Modules follow UniqueId semantics.
///
class FrameId {
public:
  ModuleId module;   ///< Load module that this frame came from.
  uintptr_t offset;  ///< Offset into module where frame's RA pointed.

  FrameId(ModuleId m, uintptr_t offset);
  FrameId(const std::string& modname, uintptr_t offset);
  FrameId(const FrameId& other);

  ~FrameId() { }

  /// writes out raw values for this FrameId
  void write_out(std::ostream& out) const;

  /// reads a frame's raw values in from a file.  module will require
  /// translation before it is usable.
  static FrameId read_in(const ModuleId::id_map& trans, std::istream& in);

  /// Assignment.
  FrameId& operator=(const FrameId& other);

#ifdef CALLPATH_HAVE_MPI
  /// Gets size of a packed frame id for sending via MPI.
  size_t packed_size(MPI_Comm comm) const;

  /// Packs a frame onto an MPI buffer.  You will also need to send the Module id_map
  /// to the remote process to translate from remote to local module ids.
  void pack(void *buf, int bufsize, int *position, MPI_Comm comm) const;

  /// Unpacks a frame from am MPI buffer.  Requires a module id map for translating
  /// remote to local module ids.
  static FrameId unpack(const ModuleId::id_map& trans, void *buf, int bufsize, int *position, MPI_Comm comm);
#endif // CALLPATH_HAVE_MPI

private:
  friend bool operator==(const FrameId& lhs, const FrameId& rhs);
  friend bool operator<(const FrameId& lhs, const FrameId& rhs);
  friend bool operator>(const FrameId& lhs, const FrameId& rhs);
};

inline bool operator==(const FrameId& lhs, const FrameId& rhs) {
  return (lhs.module == rhs.module) && (lhs.offset == rhs.offset);
}

inline bool operator<(const FrameId& lhs, const FrameId& rhs) {
  return (lhs.module == rhs.module)
    ? lhs.offset < rhs.offset
    : lhs.module < rhs.module;
}

inline bool operator>(const FrameId& lhs, const FrameId& rhs) {
  return (lhs.module == rhs.module)
    ? lhs.offset > rhs.offset
    : lhs.module > rhs.module;
}

std::ostream& operator<<(std::ostream& out, const FrameId& fid);


/// Heavyweight comparator for FrameIds.  Compares by module name and offset
/// rather than by unique pointer value.
struct frameid_string_lt {
  bool operator()(const FrameId& lhs, const FrameId& rhs) const {
    if (lhs.module.str() == rhs.module.str()) {
      return lhs.offset < rhs.offset;
    } else {
      return lhs.module.str() < rhs.module.str();
    }
  }
};


#endif //FRAME_ID_H
