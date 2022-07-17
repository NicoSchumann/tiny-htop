#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

///
/// constructor
///
Process::Process(int pid, System *system) : pid_(pid), isAlive_(true), pSystem_(system) {
  try {
   command_ = LinuxParser::Command(pid_);
   Update();
  } catch (std::runtime_error& e) {
    throw std::runtime_error("In Process::Process():" + std::string(e.what()));
  }
}

///
/// Uptates the process' object data.
///
void Process::Update() {
  activeJiffiesOld_ = activeJiffiesNew_;
  try {
    upTime_ = LinuxParser::UpTime(pid_);  // measured in seconds
    activeJiffiesNew_ = LinuxParser::ActiveJiffies(pid_);

  } catch (std::exception& e) {
    throw std::runtime_error("Process::Update():" + std::string(e.what()));
  }
}

///
/// Return this process's ID.
///
int Process::Pid() const { return pid_; }

///
/// TODO: Return this process's CPU utilization
///
float Process::CpuUtilization() const {
  return float(activeJiffiesNew_ - activeJiffiesOld_) / pSystem_->Cpu().JiffiesDelta();
}

///
/// Returns the command that generated this process.
///
string Process::Command() const { return LinuxParser::Command(pid_); }

///
/// TODO: Returns this process's memory utilization.
///
string Process::Ram() const { return LinuxParser::Ram(pid_); }

/// TODO: Return the user (name) that generated this process
string Process::User() const { return LinuxParser::User(pid_); }

///
/// Returns the age of this process (in seconds).
///
long int Process::UpTime() const { return upTime_; }

///
/// Compares two processes' CPU utilization values.
///
bool Process::operator<(Process const& other) const {
  return this->CpuUtilization() < other.CpuUtilization();
}

///
/// Detects whether a process is still valid.
///
bool Process::is_alive() const { return isAlive_; }

///
/// Switches the valid state of a process on or off.
///
void Process::set_alive(bool flag) { isAlive_ = flag; }

