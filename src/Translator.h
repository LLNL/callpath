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
#ifndef CALLPATH_TRANSLATOR_H
#define CALLPATH_TRANSLATOR_H

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "FrameInfo.h"
#include "Callpath.h"
#include "ModuleId.h"

class symtab_info;

///
/// Class to turn FrameIds into more useful FrameInfo (with symbol data).
///
class Translator { 
public:
  /// Construct a translator; optionally provide the location of the executable 
  /// for translating frames with unknown modules.
  Translator(const std::string& exe = "");
  ~Translator();

  /// Given a module/offste frame, get FrameInfo with symbol information.
  FrameInfo translate(const FrameId& frame);

  /// Given a callpath writes out the names of all the symbols in it, nicely formatted.
  void write_path(std::ostream& out, const Callpath& path, bool one_line=false, std::string indent="");

  /// Given a callpath writes out the names of all the symbols in it, nicely formatted.
  void write_path(std::ostream& out, const Callpath& path, std::string indent);

  /// Set the executable.
  void set_executable(const std::string& exe);
  
  /// Should be true if frames contain the return address and not the actual callsite.
  /// This will cause the translator to subtract one from the address when translating, to
  /// get the line info for the callsite instead of the line just after it.
  void set_callsite_mode(bool mode);
  
private:
  /// Main executable 
  ModuleId executable;

  /// Lookup table for symtabs we've already parsed.
  typedef std::map<ModuleId, symtab_info*> cache;

  /// Cache of all symbtabs seen so far.
  cache symtabs;

  /// Reads in a symbol table for the module specified.  Aborts on failure.
  symtab_info *get_symtab_info(ModuleId module);

  /// Cleans up symtab info
  void cleanup_symtab_info();

  /// whether we're in callsitemode (see set_callsite_mode()).  Defaults to true.
  bool callsite_mode;
}; // Translator

#endif // CALLPATH_TRANSLATOR_H


