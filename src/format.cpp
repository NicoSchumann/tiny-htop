#include "format.h"

#include <string>

using std::string;

///
/// Formats to HH:MM:SS string.
///
string Format::ElapsedTime(long seconds) {
  int h = seconds / 3600;                                 // hours
  int m = seconds / 60 % 60;                              // minutes
  int s = seconds % 60;                                   // seconds
  return (h < 10 ? "0" : "") + std::to_string(h) + ":"    // hours
         + (m < 10 ? "0" : "") + std::to_string(m) + ":"  // minutes
         + (s < 10 ? "0" : "") + std::to_string(s);       // seconds
}