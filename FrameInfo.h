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
