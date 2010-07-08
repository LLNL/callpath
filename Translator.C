#include  "Translator.h"
#include <sstream>
#include <iostream>
#include <fstream>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SYMTAB
#include "Symtab.h"
#include "Symbol.h"
using namespace Dyninst::SymtabAPI;
#endif // HAVE_SYMTAB

#include "io_utils.h"
using wavelet::exists;
using namespace std;

Translator::Translator(const string& exe) 
  : executable(exe), callsite_mode(true) { }


#ifndef HAVE_SYMTAB

// Just return an empty frameinfo if we don't have symtabAPI
FrameInfo Translator::translate(const FrameId& frame) {
  return FrameInfo(frame.module, frame.offset);
}

void Translator::cleanup_symtab_info() { }

#else // HAVE_SYMTAB

struct symbol_addr_gt {
  bool operator()(Symbol* lhs, Symbol *rhs)   { return (lhs->getAddr() > rhs->getAddr()); }
  bool operator()(uintptr_t lhs, Symbol *rhs) { return (lhs > ((uintptr_t) rhs->getAddr())); }
  bool operator()(Symbol* lhs, uintptr_t rhs) { return (((uintptr_t)lhs->getAddr()) > rhs); }
};


class symtab_info {
  auto_ptr<Dyninst::SymtabAPI::Symtab> symtab;
  std::vector<Dyninst::SymtabAPI::Symbol*> syms;
public:
  symtab_info(const string& filename) { 
    Symtab *st;
    if (!exists(filename.c_str()) || !Symtab::openFile(st, filename)) {
      st = NULL;
    }
    symtab.reset(st);
  }

  ~symtab_info() { }

  bool getSourceLines(vector<LineNoTuple>& lines, uintptr_t offset) {
    return symtab.get() && symtab->getSourceLines(lines, offset);
  }

  /// This gets a symbol name from an offset the same way stackwalker does it.
  void getName(uintptr_t offset, string& name) {
    if (!symtab.get()) {
      ostringstream info;
      info << "??";
      name = info.str();
      return;
    }

    if (!syms.size()) {
      if (!symtab->getAllSymbols(syms)) {
        cerr << "ERROR: couldn't read symbols from " << symtab->file() << endl;
        return;
      }
      sort(syms.begin(), syms.end(), symbol_addr_gt());
    }

    Symbol *sym =
      *lower_bound(syms.begin(), syms.end(), offset, symbol_addr_gt());

    name = sym->getTypedName();
    if (!name.length())
      name = sym->getPrettyName();
    if (!name.length())
      name = sym->getName();
  }
};


FrameInfo Translator::translate(const FrameId& frame) {
  vector<LineNoTuple> lines;

  ModuleId module = frame.module;
  if (!module) module = executable;
  symtab_info *stinfo = get_symtab_info(module);
    
  // Subtract one from the offset here, to hackily
  // convert return address to callsite
  uintptr_t offset = frame.offset;
  uintptr_t translation_offset = offset;
  if (callsite_mode) {
    translation_offset = offset ? offset - 1 : offset;
  }

  string name;
  stinfo->getName(offset, name);

  if (stinfo->getSourceLines(lines, translation_offset)) {
    return FrameInfo(module, offset, lines[0].first, lines[0].second, name);    
  } else {
    return FrameInfo(frame.module, frame.offset, name);
  }
}


symtab_info *Translator::get_symtab_info(ModuleId module) {
  Translator::cache::iterator sti = symtabs.find(module);
  if (sti == symtabs.end()) {
    string filename = module.str();
    sti = symtabs.insert(Translator::cache::value_type(module, new symtab_info(module.str()))).first;
  }

  return sti->second;
}

void Translator::cleanup_symtab_info() {
  // need to free all the symtab infos we created.
  for (Translator::cache::iterator sti = symtabs.begin(); sti != symtabs.end(); sti++) {
    delete sti->second;
  }
}

#endif // HAVE_SYMTAB

Translator::~Translator() { 
  cleanup_symtab_info();
}


/// Given a callpath writes out the names of all the symbols in it.
void Translator::write_path(ostream& out, const Callpath& path, bool one_line, std::string indent) {
  if (!path.size()) {
    out << "null_callpath";
    if (!one_line) {
      out << endl;
    }
    return;
  }
  
//Fails under AIX  FrameInfo infos[path.size()];
  vector<FrameInfo> infos(path.size());
  
  // find max field widths for the output.
  size_t max_file = 0;
  size_t max_line = 0;
  size_t max_sym = 0;
  
  for (int i=path.size()-1; i >= 0; i--) {
    infos[i] = translate(path[i]);
    max_file = max(max_file, infos[i].file.size());
    max_line = max(max_line, infos[i].line_num.size());
    max_sym  = max(max_sym,  infos[i].sym_name.size());
  }

  if (one_line) {
    if (path.size()) {
      out << infos[path.size()-1];
    }

    for (int i=path.size()-2; i >= 0; i--) {
      out << " : " << infos[i];
    }

  } else {
    size_t file_line_width = max_file+max_line + 3;
    size_t max_sym_width = max_sym + 2;

    for (size_t i=0; i < path.size(); i++) {
      out << indent;
      infos[i].write(out, file_line_width, max_sym_width);
      out << endl;
    }
    out << endl;
  }
}

void Translator::write_path(ostream& out, const Callpath& path, std::string indent) {
  write_path(out, path, false, indent);
}

void Translator::set_executable(const std::string& exe) {
  executable = ModuleId(exe);
}

void Translator::set_callsite_mode(bool mode) {
  callsite_mode = mode;
}
