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
#ifndef FRAME_ID_H
#define FRAME_ID_H

#include <stdint.h>
#include <string>
#include <set>
#include <iostream>

#include "libra-config.h"
#ifdef LIBRA_HAVE_MPI
#include <mpi.h>
#endif // LIBRA_HAVE_MPI

#include "ModuleId.h"

///
/// Module, offset representation for Callpath frames.  
/// Modules follow UniqueId semantics.
///
class FrameId {
public:
  ModuleId module;
  uintptr_t offset;

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

#ifdef LIBRA_HAVE_MPI
  /// Gets size of a packed frame id for sending via MPI.
  size_t packed_size(MPI_Comm comm) const;

  /// Packs a frame onto an MPI buffer.  You will also need to send the Module id_map
  /// to the remote process to translate from remote to local module ids.
  void pack(void *buf, int bufsize, int *position, MPI_Comm comm) const;

  /// Unpacks a frame from am MPI buffer.  Requires a module id map for translating
  /// remote to local module ids.
  static FrameId unpack(const ModuleId::id_map& trans, void *buf, int bufsize, int *position, MPI_Comm comm);
#endif // LIBRA_HAVE_MPI  
  
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
