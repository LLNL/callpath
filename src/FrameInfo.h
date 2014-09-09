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
#ifndef FRAME_INFO_H
#define FRAME_INFO_H

#include <string>
#include <iostream>
#include "FrameId.h"

struct FrameInfo {
  ModuleId module;
  std::string offset;
  std::string file;
  std::string line_num;
  std::string sym_name;

  FrameInfo();
  FrameInfo(ModuleId module, uintptr_t offset, std::string name="");
  FrameInfo(ModuleId module, uintptr_t offset,
            const std::string& filename, int line, const std::string& sym_name);
  ~FrameInfo();

  void write(std::ostream& out, size_t file_line_width=0, size_t sym_width=0) const;
};

inline std::ostream& operator<<(std::ostream& out, const FrameInfo& info) {
  info.write(out);
  return out;
}

#endif // FRAME_INFO_H
