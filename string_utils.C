#include "string_utils.h"
#include <iostream>
#include <sstream>
using namespace std;

namespace stringutils {
  void split(const string& str, const string& delim, vector<string>& parts) {
    size_t start, end = 0;
    
    while (end < str.size()) {
      start = end;
      while (start < str.size() && (delim.find(str[start]) != string::npos))
        start++;  // skip initial whitespace
      
      end = start;
      while (end < str.size() && (delim.find(str[end]) == string::npos))
        end++; // skip to end of word
      
      if (end-start != 0) {  // just ignore zero-length strings.
        parts.push_back(string(str, start, end-start));
      }
    }
  }

  void split_str(const string& str, const string& delim, vector<string>& parts) {
    size_t start = 0;
    size_t end = 0;
    
    while (start < str.size()) {
      end = str.find(delim, start);
      if (end == string::npos) end = str.size();

      if (end-start != 0) {  // just ignore zero-length strings.
        parts.push_back(string(str, start, end-start));
      }
      
      start = end + delim.size();
    }
  }

  string trim(const string& str, const string chars) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < str.size() && chars.find(str[start]) != string::npos) 
      start++;
    
    while (end > 0 && chars.find(str[end-1]) != string::npos) {
      end--;
    }
    
    return string(str, start, end - start);
  }

  string times(const string& str, size_t n) {
    ostringstream s;
    for (size_t i=0; i < n; i++) {
      s << str;
    }
    return s.str();
  }

}  // namespace

