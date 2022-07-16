#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <unordered_map>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();                   /// The CPU.
  std::vector<Process>& Processes();  /// List of processes.
  float
  MemoryUtilization();  /// Total memory utiliation in fraction of total memory.
  long UpTime();        /// System uptime in seconds.
  int TotalProcesses();           /// Count of total processes.
  int RunningProcesses();         /// How much processes are running.
  std::string Kernel();           /// Name of the kernel.
  std::string OperatingSystem();  /// Name of the operating system.

  System();  /// Initializes the processes.

  void Update();  /// Uptates the values.

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};  /// List which holds all processes.
  std::unordered_map<int, Process*> procMap_ = {};  /// PID to Process mapping.
};

#endif