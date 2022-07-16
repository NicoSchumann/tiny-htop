#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() {
  std::vector<int> pids = LinuxParser::Pids();
  for (int pid : pids) {
    Process process(pid);
    processes_.push_back(process);
    procMap_[pid] = &process;
  }
}

Processor& System::Cpu() { return cpu_; }

///
/// Return a container composed of the system's processes
///
std::vector<Process>& System::Processes() { return processes_; }

///
/// Return the system's kernel identifier (string).
///
std::string System::Kernel() { return LinuxParser::Kernel(); }

///
/// Returns the system's memory Utilization in percent (0.0f-1.0f).
///
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

///
/// Return the operating system name.
///
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

///
/// Return the number of processes actively running on the system.
///
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

///
/// Return the total number of processes on the system.
///
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

///
/// Returns the number of seconds since the system started running
///
long int System::UpTime() { return LinuxParser::UpTime(); }

///
/// TODO: Updates the process.
///
void System::Update() {
  // Set all processes to dead state.
  for (Process& p : processes_) {
    p.set_alive(false);
  }

  std::vector<int> pids = LinuxParser::Pids();

  // Set all processes from the pid list wich are still valid to living state
  // and add all processes which are not yet into this list.
  for (int pid : pids) {
    if (procMap_.contains(pid)) {
      procMap_[pid]->set_alive(true);
    } else {
      // Add a new process if it is not yet in the pid list.
      Process process(pid);
      processes_.push_back(process);
      procMap_[pid] = &process;
    }
  }

  // Delete all process entries which doesn't exist in the pid list.
  for (auto itr = processes_.begin(); itr != processes_.end();) {
    if (!(itr->is_alive())) {
      procMap_.erase(itr->Pid());
      itr = processes_.erase(itr);
    } else {
      ++itr;
    }
  }

  // Updates all processes.
  for (auto& process : processes_) {
    process.Update();
  }

  std::sort(processes_.begin(), processes_.end(),
            [](auto& a, auto& b) { return b < a; });
}