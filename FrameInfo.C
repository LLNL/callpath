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
#include "FrameInfo.h"

#include <sstream>
#include <iomanip>
using namespace std;

  
FrameInfo::FrameInfo(ModuleId m, uintptr_t o, const string& f, int l, const string& n) 
  : module(m), file(f), sym_name(n) { 

  ostringstream ln;
  ln << l;
  line_num = ln.str();
    
  ostringstream off;
  off << "(0x" << hex << o << dec << ")";
  offset = off.str();
}
    
FrameInfo::FrameInfo() : module() { }

FrameInfo::FrameInfo(ModuleId m, uintptr_t o, string n) : module(m), sym_name(n) {
  ostringstream off;
  off << "(0x" << hex << o << dec << ")";
  offset = off.str();
}

FrameInfo::~FrameInfo() { }
    
void FrameInfo::write(ostream& out, size_t file_line_width, size_t sym_width) const {
  if (file == "") {
    out << left << setw(file_line_width) << "??";
  } else {
    ostringstream file_line;
    file_line << file + ":" + line_num;
    out << left << setw(file_line_width) << file_line.str();
  }
  if (!file_line_width) out << ":";

  if (sym_name == "") {
    out << left << setw(sym_width) << "??";
  } else {
    out << left << setw(sym_width) << sym_name;
  }
  out << " ";

  out << module ? module.c_str() : "[unknown module]";
  out << offset;
}
  
