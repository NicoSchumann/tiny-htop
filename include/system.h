#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class Process;

class System {
 public:
  Processor& Cpu();                   /// The CPU.
  std::vector<Process>& Processes();  /// List of processes.
  Process& Pid(int pid);              /// Finds a process by PID.
  float
  MemoryUtilization();  /// Total memory utiliation in fraction of total memory.
  long UpTime();        /// System uptime in seconds.
  int TotalProcesses();           /// Count of total processes.
  int RunningProcesses();         /// How much processes are running.
  std::string Kernel();           /// Name of the kernel.
  std::string OperatingSystem();  /// Name of the operating system.

  System();  /// Constructor. Initializes the processes.

  void Update();  /// Uptates the values.

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};  /// List which holds all processes.
  // std::vector<int> validPids_ = {};  /// All valid PID.
};

#endif