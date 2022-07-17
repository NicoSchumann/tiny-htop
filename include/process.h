#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class System;

#include "system.h"

///
/// Objects instantiated by this class representing processes running at the
/// operating system.
///
class Process {
 public:
  int Pid() const;               /// The process-identifying-number.
  std::string User() const;      /// The user name to which belongs the process.
  std::string Command() const;   /// The command, which started the process.
  float CpuUtilization() const;  /// Self explaining. Val between 0.0 and 1.0.
  std::string Ram() const;       /// How many RAM the process occupies (MB).
  long int UpTime() const;       /// How many seconds the process still runs.
  bool operator<(Process const& other) const;  /// Implementation dependent.

  Process(int pid, System *system);
  void Update();
  bool is_alive() const;  /// Returns true if the process lives.
  void set_alive(bool);   /// Set the living state of the process.

 private:
  int pid_;       /// process ID
  bool isAlive_;  /// If the process is no zombi
  long upTime_;   /// Process' up-time, measured in seconds.
  std::string command_;  /// Command which started the process.
  System * pSystem_;

  long activeJiffiesOld_{};  // Measured in clock ticks,
  long activeJiffiesNew_{};  // both vals used to calc the delta.
};

#endif