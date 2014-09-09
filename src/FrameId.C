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
#include "FrameId.h"

#include "callpath-config.h"

#ifdef CALLPATH_HAVE_MPI
#include "mpi_utils.h"
#endif // CALLPATH_HAVE_MPI

#include "io_utils.h"
#include <iomanip>
using namespace io_utils;
using namespace std;


FrameId::FrameId(ModuleId mod, uintptr_t off)
  : module(mod), offset(off) { }

FrameId::FrameId(const string& modname, uintptr_t off)
  : module(modname), offset(off) { }

FrameId::FrameId(const FrameId& other)
  : module(other.module), offset(other.offset) { }

FrameId& FrameId::operator=(const FrameId& other) {
  module = other.module;
  offset = other.offset;
  return *this;
}

void FrameId::write_out(ostream& out) const {
  module.write_id(out);
  vl_write(out, offset);
}

FrameId FrameId::read_in(const ModuleId::id_map& trans, istream& in) {
  FrameId fid(ModuleId::read_id(trans, in), 0);
  fid.offset = vl_read(in);      // force eval order of reads.
  return fid;
}

ostream& operator<<(ostream& out, const FrameId& fid) {
  out << fid.module << "(0x" << hex << fid.offset << ")" << dec;
  return out;
}


#ifdef CALLPATH_HAVE_MPI

size_t FrameId::packed_size(MPI_Comm comm) const {
  size_t pack_size = 0;
  pack_size += module.packed_size_id(comm);            // module pointer
  pack_size += pmpi_packed_size(1, MPI_UINTPTR_T, comm);  // offset
  return pack_size;
}

void FrameId::pack(void *buf, int bufsize, int *position, MPI_Comm comm) const {
  module.pack_id(buf, bufsize, position, comm);
  PMPI_Pack(const_cast<uintptr_t*>(&offset), 1, MPI_UINTPTR_T, buf, bufsize, position, comm);
}

FrameId FrameId::unpack(const ModuleId::id_map& trans, void *buf, int bufsize, int *position, MPI_Comm comm) {
  FrameId result(ModuleId::unpack_id(trans, buf, bufsize, position, comm), 0);
  PMPI_Unpack(buf, bufsize, position, &result.offset, 1, MPI_UINTPTR_T, comm);
  return result;
}

#endif // CALLPATH_HAVE_MPI

