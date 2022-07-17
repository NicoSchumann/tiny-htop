
#include "system.h"

#include <algorithm>
#include <iostream>  // DEBUG
#include <stdexcept>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

///
/// Constructor
///
System::System() {
  std::vector<int> pids = LinuxParser::Pids();

  for (int pid : pids) {
    processes_.emplace_back(pid, this);
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
/// Returns the number of seconds since the system sarted running.
///
long int System::UpTime() { return LinuxParser::UpTime(); }

Process& System::Pid(int pid) {
  auto itr = std::find_if(processes_.begin(), processes_.end(),
                          [=](auto& p) { return p.Pid() == pid; });
  if (itr == processes_.end()) {
    throw std::runtime_error("System::Pid(int)");
  }
  return *itr;
}

///
/// Updates the system. I.e. mainly the process list.
///
void System::Update() {
  std::vector<int> new_pids = LinuxParser::Pids();
  auto& prc = processes_;  // short handle

  // 1) Checks for each process whether it's in the actual PID list.
  //    - if not, drop dead.
  for (Process& p : prc) {
    bool flag = false;
    for (const int pid : new_pids) {
      if (p.Pid() == pid) {
        flag = true;
        break;
      }
    }
    if (flag == false) {
      p.drop_dead();
    }
  }
  // Erase all dead-dropped processes
  for (auto itr = prc.begin(); itr != prc.end();) {
    if (itr->is_dead()) {
      itr = prc.erase(itr);
    } else {
      ++itr;
    }
  }

  // 2) Checking for each PID, wheter fore it
  //    exists a process.
  //    - if not, create one.
  for (const int pid : new_pids) {
    bool flag = false;
    for (const Process& p : prc) {
      if (pid == p.Pid()) {
        flag = true;
        break;
      }
    }
    if (flag == false) {
      prc.emplace_back(pid, this);
    }
  }

  // Updates all processes.
  for (auto& process : prc) {
    process.Update();
  }
  std::sort(prc.begin(), prc.end(), [&](auto& a, auto& b) { return b < a; });
}