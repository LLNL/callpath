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
#ifndef UNIQUE_ID_H
#define UNIQUE_ID_H

#include <stdint.h>
#include <string>
#include <set>
#include <map>
#include <ostream>

#include "safe_bool.h"
#include "io_utils.h"

#include "callpath-config.h"
#ifdef CALLPATH_HAVE_MPI
#include <mpi.h>
#include "mpi_utils.h"
#endif // CALLPATH_HAVE_MPI

/// Compares by targets of pointer types.
struct dereference_lt {
  template <typename Tp>
  bool operator()(Tp lhs, Tp rhs) const {
    return *lhs < *rhs;
  }
};

/// Class to represent internally-uniqued strings.  Much like symbols in ruby or lisp,
/// this keeps an internal set of pointers to unique std::strings.
///
/// Since instances of UniqueId contain unique pointers (looked up on creation), they
/// can compared fast for equality, less than, etc.
/// Methods for serialization, etc are also provided here.
///
/// To make your own unique id class using UniqueId, use static polymorphism:
///
///    class MyUniqueIdClass : public UniqueId<MyUniqueIdClass> {
///    public:
///        MyUniqueIdClass(const std::string& id) : UniqueId<MyUniqueIdClass>(id) { }
///    };
///
/// That's all!
///
template <class Derived>
class UniqueId
  : public safe_bool< UniqueId<Derived> >
{
public:
  /// Type for set of unique uid values.
  typedef std::set<const std::string*, dereference_lt> id_set;
  typedef typename id_set::iterator id_set_iterator;

  /// Map type for translating ids from remote processes.  This maps unintptr_t's
  /// (remote pointer values) to local string*'s.  It needs to be specially constructed
  /// using static routines below.
  typedef std::map<uintptr_t, Derived> id_map;

protected:
  /// Unique identifier for this instance of the UniqueId
  const std::string *identifier;

  static id_set& get_identifiers() {
    static id_set ids;
    return ids;
  }

  const std::string *lookup(const std::string& id) {
    id_set& ids = get_identifiers();
    id_set_iterator i = ids.find(&id);
    if (i == ids.end()) {
      i = ids.insert(new std::string(id)).first;
    }
    return *i;
  }

  /// Raw pointer constructor.  Used internally for serialization.
  UniqueId(const std::string *id) : identifier(id) { }

  /// Construct a Null UniqueId.  Subclasses can choose to expose this or not.
  UniqueId() : identifier(lookup("")) { }

  /// Constructor takes a const std::string reference, gets a unique pointer to its value,
  /// and inits this uid with the pointer.
  UniqueId(const std::string& id) : identifier(lookup(id)) { }

public:
  /// test method for safe_bool
  bool boolean_test() const {
    return !identifier->empty();
  }

  Derived& operator=(const Derived& other) {
    return identifier = other->identifier;
  }

  bool operator<(const Derived& other) const {
    return identifier < other.identifier;
  }

  bool operator==(const Derived& other) const {
    return identifier == other.identifier;
  }

  bool operator!=(const Derived& other) const {
    return identifier != other.identifier;
  }

  bool operator>(const Derived& other) const {
    return identifier > other.identifier;
  }

  const char *c_str() const {
    return identifier->c_str();
  }

  const std::string& str() const {
    return *identifier;
  }

  void write_out(std::ostream& out) const {
    io_utils::vl_write(out, identifier->size());
    out.write(identifier->c_str(), identifier->size());
  }

  static Derived read_in(std::istream& in) {
    size_t id_size = io_utils::vl_read(in);
    char buf[id_size+1];
    in.read(buf, id_size);
    buf[id_size] = '\0';
    return Derived(buf);
  }

  void write_id(std::ostream& out) const {
    io_utils::vl_write(out, reinterpret_cast<uintptr_t>(identifier));
  }

  static Derived read_id(const id_map& trans, std::istream& in) {
    uintptr_t id = io_utils::vl_read(in);
    return trans.find(id)->second;
  }

  template<class D>
  friend std::ostream& operator<<(std::ostream& out, UniqueId<D> uid);

#ifdef CALLPATH_HAVE_MPI
  // ----------------------------------------------------------------------------------
  // These routines pack UniqueIds as raw strings.  Receiver process looks up received
  // strings and returns UniqueIds.
  // ----------------------------------------------------------------------------------
  int packed_size(MPI_Comm comm) const {
    int size = 0;
    size += pmpi_packed_size(1, MPI_INT, comm);  // identifier size
    size += pmpi_packed_size(identifier->size(), MPI_CHAR, comm);
    return size;
  }


  void pack(void *buf, int bufsize, int *position, MPI_Comm comm) const {
    int size = identifier->size();
    PMPI_Pack(&size, 1, MPI_INT, buf, bufsize, position, comm);
    if (size) {
      char *casted = const_cast<char*>(identifier->c_str());
      PMPI_Pack(casted, identifier->size(), MPI_CHAR, buf, bufsize, position, comm);
    }
  }


  static Derived unpack(void *buf, int bufsize, int *position, MPI_Comm comm) {
    int size;
    PMPI_Unpack(buf, bufsize, position, &size, 1, MPI_INT,  comm);

    char idstring[size+1];
    if (size) PMPI_Unpack(buf, bufsize, position, idstring, size, MPI_CHAR,  comm);
    idstring[size] = '\0';

    return Derived(idstring);
  }

  // ----------------------------------------------------------------------------------
  // Below routines pack UniqueIds by id.  They require that you first send an id_map
  // that the receiver can use to translate remote ids, but can be more efficient than
  // sending raw strings as above.  See below for routines for transferring id_maps.
  // ----------------------------------------------------------------------------------

  /// Returns size of raw identifier.
  size_t packed_size_id(MPI_Comm comm) const {
    return pmpi_packed_size(1, MPI_UINTPTR_T, comm);
  }

  /// Packs raw identifier onto a buffer.  Receiver will need an id_map to translate.
  void pack_id(void *buf, int bufsize, int *position, MPI_Comm comm) const {
    const std::string** casted = const_cast<const std::string**>(&identifier);
    PMPI_Pack(casted, 1, MPI_UINTPTR_T, buf, bufsize, position, comm);
  }

  /// Unpacks remote raw identifier and builds a local unique id using the id_map supplied.
  static Derived unpack_id(const id_map& remote_to_local, void *buf, int bufsize, int *position, MPI_Comm comm) {
    uintptr_t remote_addr;
    PMPI_Unpack(buf, bufsize, position, &remote_addr, 1, MPI_UINTPTR_T, comm);
    return remote_to_local.find(remote_addr)->second;
  }

  // ----------------------------------------------------------------------------------
  // Below routines are for sending id_maps between processes.
  // ----------------------------------------------------------------------------------
  /// packed size of entire buffer full of id_map
  static size_t packed_size_id_map(MPI_Comm comm) {
    size_t size = 0;
    size += pmpi_packed_size(1, MPI_INT, comm);                // number of mappings

    id_set& ids = get_identifiers();
    for (id_set_iterator i=ids.begin(); i != ids.end(); i++) {
      size += pmpi_packed_size(1, MPI_UINTPTR_T, comm);     // local addr of module string
      size += UniqueId<Derived>(*i).packed_size(comm);     // size of raw string
    }
    return size;
  }

  /// Sends all known pointer/identifier mappings to anther process.
  static void pack_id_map(void *buf, int bufsize, int *position, MPI_Comm comm) {
    int len = get_identifiers().size();
    PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

    id_set& ids = get_identifiers();
    for (id_set_iterator i=ids.begin(); i != ids.end(); i++) {
      std::string **addr = const_cast<std::string**>(&(*i));      // local addr of module string
      PMPI_Pack(addr, 1, MPI_UINTPTR_T, buf, bufsize, position, comm);
      UniqueId<Derived>(*i).pack(buf, bufsize, position, comm);   // raw string.
    }
  }

  /// Receies identifier mappings from another process.  Builds an id_map (translation table)
  /// that can be used to unpack UniqueIds in bulk.
  static void unpack_id_map(void *buf, int bufsize, int *position, id_map& dest, MPI_Comm comm) {
    int len;
    PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);

    for (int i=0; i < len; i++) {
      uintptr_t remote_addr;     // addr of string on remote machine
      PMPI_Unpack(buf, bufsize, position, &remote_addr, 1, MPI_UINTPTR_T, comm);

      // unpack, look up, and add mapping for raw identifier from remote process.
      dest.insert(typename id_map::value_type(remote_addr, Derived::unpack(buf, bufsize, position, comm)));
    }
  }

#endif // CALLPATH_HAVE_MPI
};


template<class Derived>
std::ostream& operator<<(std::ostream& out, UniqueId<Derived> uid) {
  out << uid.str();
  return out;
}



#endif //UNIQUE_ID_H
