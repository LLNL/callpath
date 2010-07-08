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
#include "FrameId.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#ifdef LIBRA_HAVE_MPI
#include "mpi_utils.h"
#endif // LIBRA_HAVE_MPI

#include "io_utils.h"
#include <iomanip>
using namespace wavelet;
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


#ifdef LIBRA_HAVE_MPI

size_t FrameId::packed_size(MPI_Comm comm) const {
  size_t pack_size = 0;
  pack_size += module.packed_size_id(comm);            // module pointer
  pack_size += mpi_packed_size(1, MPI_UINTPTR_T, comm);  // offset
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

#endif // LIBRA_HAVE_MPI

