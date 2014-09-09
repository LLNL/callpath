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

