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
#ifndef CALLPATH_H
#define CALLPATH_H

#include "callpath-config.h"
#ifdef CALLPATH_HAVE_MPI
#include <mpi.h>
#endif // CALLPATH_HAVE_MPI

#include <stdint.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "FrameId.h"
#include "ModuleId.h"

/// Container for FrameIds, representing a callpath.
/// Currently, Callpaths are created via StackwalkerAPI in CallpathRuntime.
///
/// Features:
/// - Reading/writing to streams.
/// - Send/receive via MPI.
/// - Fast comparison and equality operators.
///
class Callpath : public safe_bool<Callpath> {
public:

  Callpath() : path(NULL) { }       ///< Construct a null callpath.

  Callpath(const Callpath& other);  ///< Copy constructor

  ~Callpath() { }

  /// Assignment
  Callpath& operator=(const Callpath& other);

  /// Dumps all known paths to a file
  static void dump(std::ostream& out);

  static Callpath create(const std::vector<FrameId>& path);

  /// Gets the ith element in the callpath.
  const FrameId& operator[](size_t i) const {
    return (*path)[i];
  }

  /// Synonym for operator[], but with bounds checking.
  const FrameId& get(size_t i) const;

  /// Number of elements in the callpath.
  size_t size() const;

  /// Writes this callpath out to a stream.
  void write_out(std::ostream& out);

  /// True if other is a prefix of this callpath.
  bool in(const Callpath& other) const;

  /// Returns a new callpath containing a slice of this callpath: [start, end).
  /// TODO: allow callpaths to be managed/unmanaged (at runtime it might make sense to
  ///       unique things but here it really doesn't.)
  Callpath slice(size_t start, size_t end);

  /// Version of slice with end assumed to be size().
  Callpath slice(size_t start);

  /// Reads a callpath in from a stream.
  static Callpath read_in(std::istream& in);

  /// True if the callpath is not null
  bool boolean_test() const {
    return path;
  }

#ifdef CALLPATH_HAVE_MPI
  /// Gets upper bound on size of this callpath, if it were packed into an MPI buffer.
  size_t packed_size(MPI_Comm comm) const;

  /// Packs this callpath into an MPI transport buffer.
  void pack(void *buf, int bufsize, int *position, MPI_Comm comm) const;

  /// Unpacks a callpath packed by pack().  Ensures path pointer uniqueness
  /// within processes, but not across nodes.  Requires module translation via
  /// a map from send_modules().
  static Callpath unpack(const ModuleId::id_map& modules, void *buf, int bufsize, int *position, MPI_Comm comm);

#endif // CALLPATH_HAVE_MPI

private:
  /// Unique, null-terminated array of symbol_ids for this callpath
  const std::vector<FrameId> *path;

  /// Private value constructor: used only by this class.
  Callpath(const std::vector<FrameId> *path);

  // Declare operators as friends so they can get at the internals.
  friend std::ostream& operator<<(std::ostream& out, const Callpath& path);
  friend bool operator==(const Callpath& lhs, const Callpath& rhs);
  friend bool operator<(const Callpath& lhs, const Callpath& rhs);
  friend bool operator>(const Callpath& lhs, const Callpath& rhs);
  friend struct callpath_path_lt;
}; // Callpath


/// Uses fast pointer comparison b/c path is guaranteed unique per callpath.
inline bool operator==(const Callpath& lhs, const Callpath& rhs) {
  return lhs.path == rhs.path;
}

/// Uses fast pointer comparison b/c path is guaranteed unique per callpath.
inline bool operator>(const Callpath& lhs, const Callpath& rhs) {
  return lhs.path > rhs.path;
}

/// Uses fast pointer comparison b/c path is guaranteed unique per callpath.
inline bool operator<(const Callpath& lhs, const Callpath& rhs) {
  return lhs.path < rhs.path;
}

/// Uses fast pointer comparison b/c path is guaranteed unique per callpath.
inline bool operator!=(const Callpath& lhs, const Callpath& rhs) {
  return !(lhs == rhs);
}

/// Outputs a simple string representation of this callpath.
std::ostream& operator<<(std::ostream& out, const Callpath& path);


/// Heavyweight comparator for vectors of FrameIds.  Iterates over all FrameIds,
/// calling LessThan on each of them.
template <class LessThan>
struct pathvector_lt {
  LessThan lt;
  bool operator()(const std::vector<FrameId> *lhs, const std::vector<FrameId> *rhs) const {
    if (lhs == rhs)  return false;
    if (lhs == NULL) return true;
    if (rhs == NULL) return false;

    for (size_t i=0; i < lhs->size() && i < rhs->size(); i++) {
      if (lt((*lhs)[i], (*rhs)[i])) {
        return true;
      } else if (lt((*rhs)[i], (*lhs)[i])) {
        return false;
      }
    }
    return lhs->size() < rhs->size();
  }
};

/// Functor for enforcing a total ordering of callpaths across different processes.
/// Compares frames using frameid_string_lt, which does a string compare on module
/// names instead of the fast pointer compare.
struct callpath_path_lt {
  pathvector_lt<frameid_string_lt> lt;
  bool operator()(const Callpath& lhs, const Callpath& rhs) {
    return lt(lhs.path, rhs.path);
  }
};


///
/// Parse a complete callpath from a string.
///
Callpath make_path(const std::string& path);


#endif //CALLPATH_H
