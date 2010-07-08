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
