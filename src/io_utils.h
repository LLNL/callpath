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
#ifndef WT_IO_UTILS_H
#define WT_IO_UTILS_H

#include <stdint.h>
#include <cstdlib>
#include <iostream>

#include <sys/stat.h>

namespace ioutils {

  // test whether a file exists or not
  inline bool exists(const char *filename) {
    struct stat st;
    return !stat(filename, &st);
  }


  // Variable-length read and write routines for unsigned numbers.
  size_t vl_write(std::ostream& out, unsigned long long size);
  unsigned long long vl_read(std::istream& in);


  /// Endian-agnostic write for integer types. This doesn't compress
  /// like vl_write, but it handles signs.
  template<class T>
  size_t write_generic(std::ostream& out, T num) {
    for (size_t i=0; i < sizeof(T); i++) {
      unsigned char lo_bits = (num & 0xFF);
      out.write((char*)&lo_bits, 1);
      num >>= 8;
    }
    return sizeof(T);
  }


  /// Endian-agnostic read for integer types. This doesn't compress
  /// like vl_write, but it handles signs.
  template<class T>
  T read_generic(std::istream& in) {
    T num = 0;
    for (size_t i=0; i < sizeof(T); i++) {
      unsigned char byte;
      in.read((char*)&byte, 1);
      num |= ((T)byte) << (i<<3);
    }
    return num;
  }
  

  /// Test for integral types to make sure they're powers of two.
  template <class T>
  bool isPowerOf2(T num) { return !(num & (num-1)); }

  /// Returns least power of two greater than or equal to num
  inline uint64_t gePowerOf2(uint64_t num) {
    num--;
    num |= (num >> 1);  // these fill with ones.
    num |= (num >> 2);
    num |= (num >> 4);
    num |= (num >> 8);
    num |= (num >> 16);
    num |= (num >> 32);
    num++;
    return num;
  }


  /// Returns greatest power of two less than or equal to num
  inline uint64_t lePowerOf2(uint64_t num) {
    num |= (num >> 1);  // these fill with ones.
    num |= (num >> 2);
    num |= (num >> 4);
    num |= (num >> 8);
    num |= (num >> 16);
    num |= (num >> 32);
    return num - (num >> 1);
  }


  /// Takes the log base 2 of a power of 2, returns a char.
  /// Returns -1 if 0 is passed in.
  signed char log2pow2(unsigned long long powerOf2);

} //namespace

#endif // WT_IO_UTILS_H
