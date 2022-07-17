#include "processor.h"
#include "linux_parser.h"


///
/// Constructor
///
Processor::Processor() {
  jiffiesOld_.active = jiffiesNew_.active = LinuxParser::ActiveJiffies();
  jiffiesOld_.idle = jiffiesNew_.idle = LinuxParser::IdleJiffies();

  // Take care because calculating the delta, avoiding division by zero.
  ++jiffiesNew_.active;
  ++jiffiesNew_.idle;

}

///
/// Updates the jiffies, for calculating a delta.
///
void Processor::Update() {
  jiffiesOld_ = jiffiesNew_;
  jiffiesNew_.active = LinuxParser::ActiveJiffies();
  jiffiesNew_.idle = LinuxParser::IdleJiffies();
}

///
///
///
long Processor::JiffiesActiveDelta() {
  return jiffiesNew_.active - jiffiesOld_.active;
}
///
///
///
long Processor::JiffiesIdleDelta() {
  return jiffiesNew_.idle - jiffiesOld_.idle;
}

///
///
///
long Processor::JiffiesDelta() {
  return JiffiesActiveDelta() + JiffiesIdleDelta();
}
///
/// Returns the aggregate CPU utilization. Ranges go from 0.0f to 1.0f
///
float Processor::Utilization() {
  long delta_active_jiffies = JiffiesActiveDelta();
  long delta_idle_jiffies = JiffiesIdleDelta();
  return static_cast<float>(delta_active_jiffies) / (delta_active_jiffies + delta_idle_jiffies);
}

