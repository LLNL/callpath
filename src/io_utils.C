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
#include "io_utils.h"
using namespace std;

#include <cassert>

namespace ioutils {

  size_t vl_write(ostream& out, unsigned long long size) {
    int out_bytes = 0;

    do {
      unsigned long long out_byte = (long long)(size & 0x7Fll);
      if ((size >>= 7) > 0) out_byte |= 0x80;

      char oc = (char)out_byte;
      out.write(&oc, 1);

      if (!out.good()) {
        cerr << "Error: can't write to file." << endl;
        exit(1);
      }
      out_bytes++;
    } while (size);

    return out_bytes;
  }
  
  
  unsigned long long vl_read(istream& in) {
    unsigned shift = 0;
    unsigned long long long_bytes = 0;

    char ichar;
    unsigned long long file_byte = 0;

    // read variable-length header with number of code bytes
    do {
      in.read(&ichar, 1);
      file_byte = (file_byte & ~0xFF) | ichar;
      if (!in.good()) return 0;

      long_bytes |= (long long)(file_byte & 0x7Fll) << shift;
      shift += 7;
    } while (file_byte & 0x80);

    return long_bytes;
  }


  signed char log2pow2(unsigned long long powerOf2) {
    // make sure it's a power of 2.
    assert(isPowerOf2(powerOf2));
    
    signed char n = -1;
    while (powerOf2 > 0) {
      powerOf2 >>= 1;
      n++;
    }

    return n;
  }

} //namespace
