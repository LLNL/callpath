#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>

namespace stringutils {

  /// Breaks a string into substrings using ANY characters in <delim> as delimiters.
  /// For example, to split a string by commas and whitespace, use ", " for delim.
  void split(const std::string& str, const std::string& delim, std::vector<std::string>& parts);

  /// Breaks a string into substrings using only the exact string <delim> as a delimiter.
  /// For example, to split a string by "=>", pass "=>".  
  void split_str(const std::string& str, const std::string& delim, std::vector<std::string>& parts);

  /// Trims any characters in <chars> off both ends of a string and returns the result.
  std::string trim(const std::string& str, const std::string chars = " ");
  
  /// Creates a string containing n times a particular string
  std::string times(const std::string& str, size_t n);

} // namespace

#endif //STRING_UTILS_H
