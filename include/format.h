#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {

///
/// Formats the time from seconds to HH:MM::SS
///
std::string ElapsedTime(long times);
};  // namespace Format

#endif