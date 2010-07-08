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
  
