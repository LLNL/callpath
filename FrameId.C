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

