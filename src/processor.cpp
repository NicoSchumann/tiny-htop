#include "processor.h"

#include <cmath>

#include "linux_parser.h"

///
/// Returns the aggregate CPU utilization. Ranges go from 0.0f to 1.0f.
///
float Processor::Utilization() {
  static int idx = 0;

  jiffies_[idx].active = LinuxParser::ActiveJiffies();
  jiffies_[idx].idle = LinuxParser::IdleJiffies();

  long delta_active_jiffies = std::abs(
      jiffies_[idx].active - jiffies_[(idx + 1) % jiffies_.size()].active);
  long delta_idle_jiffies =
      std::abs(jiffies_[idx].idle - jiffies_[(idx + 1) % jiffies_.size()].idle);

  ++idx;
  idx %= jiffies_.size();

  return static_cast<float>(delta_active_jiffies) /
         (delta_active_jiffies + delta_idle_jiffies);
}

Processor::Processor() {
  long active_jiffies = LinuxParser::ActiveJiffies();
  long idle_jiffies = LinuxParser::IdleJiffies();

  // We take care that both are different, for avoiding division by zero in
  // Utilization().
  for (std::size_t i{}; i < jiffies_.size(); ++i) {
    jiffies_[i] = {active_jiffies + (long)i, idle_jiffies + (long)i};
  }
}