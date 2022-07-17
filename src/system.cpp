
#include <algorithm>
#include <string>
#include <vector>
#include <stdexcept>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

///
/// Constructor
///
System::System() {
  validPids_ = LinuxParser::Pids();

  for (int pid : validPids_) {
    processes_.emplace_back(pid,this);
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

Process & System::Pid( int pid ) {
  auto itr = std::find_if(processes_.begin(),processes_.end(), [=](auto &p){return p.Pid() == pid;});
  if( itr == processes_.end() ) { throw std::runtime_error("System::Pid(int)"); }
  return *itr;
}
///
/// Updates the system.
///
void System::Update() {

  // Updating the process list
  //
  std::vector<int> new_pids = LinuxParser::Pids();
  auto & old_pids = validPids_;  // shortcut
  auto & prc = processes_;       // shortcut
  std::sort(new_pids.begin(), new_pids.end());
  std::sort(old_pids.begin(), old_pids.end());
  std::sort(prc.begin(),prc.end(),[](auto &a, auto& b){return a.Pid() <= b.Pid();});

  while( true ) {

    auto pair = std::mismatch(new_pids.begin(), old_pids.begin(), new_pids.end(), old_pids.end());
    
    // no mismatch, breaks out of the while loop
    if( pair.first == new_pids.end() && pair.second == old_pids.end()) {
      break;
    }
    // old_pids is too large, other values are fitting.
    else if( pair.first == new_pids.end() && pair.second != old_pids.end()) {
      // delete all entries of old_pids till end
      for( ; pair.second != old_pids.end(); pair.second = old_pids.erase(pair.second))
      {
        auto itr = std::find_if(prc.begin(),prc.end(), [&](auto &p){return p.Pid() == *(pair.second);});
        if( itr != prc.end()) { prc.erase(itr); }
        else { throw std::runtime_error("System::Update():2nd_branch"); }
      }
    }
    // old_pids is to small
    else if( pair.first != new_pids.end() && pair.second == old_pids.end()) {
      // adding all missing entries
      for( ; pair.first != new_pids.end(); ++pair.first) {
        old_pids.push_back(*(pair.first));
        prc.emplace_back(*(pair.first),this);
      }
    }
      // If nothing of the above is true, then there happened a real mismatch.
    else if( pair.first != new_pids.end() && pair.second != old_pids.end()){
      old_pids.erase(pair.second);
      auto it = std::find_if(prc.begin(),prc.end(), [&](auto &p){return p.Pid() == *(pair.second);});
      if( it != prc.end()) { prc.erase(it); }
      else { throw std::runtime_error("System::Update():4th_branch"); }
    }
  }


  // Updates all processes.
  for (auto& process : processes_) {
    process.Update();
  }
  std::sort(processes_.begin(), processes_.end(), [&](auto& a, auto& b) { return b < a;});
}