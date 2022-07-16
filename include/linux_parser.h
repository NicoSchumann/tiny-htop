#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <string>
#include <vector>

namespace LinuxParser {

/// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

///
/// Returns the memory utilization in percent (0.0f-1.0f).
///
float MemoryUtilization();

///
/// System uptime in seconds.
///
long UpTime();

///
/// List of all process identifying numbers.
///
std::vector<int> Pids();

///
/// Total number of processes.
///
int TotalProcesses();

///
/// Number of running processes.
///
int RunningProcesses();

///
/// Name of the operating system.
///
std::string OperatingSystem();

///
/// Name of the system's kernel.
///
std::string Kernel();

///
/// This enum represent different states of the CPU consumtion time.
///
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

///
/// Data about utilization of the CPU.
///
std::vector<std::string> CpuUtilization();

///
/// Total numbersof jiffies of the CPU.
///
long Jiffies();

///
/// Total number of jiffies whose where used for computation.
///
long ActiveJiffies();

///
/// Total numbers of jiffies whose were used for computation of the process.
///
long ActiveJiffies(int pid);

///
/// Total numbers of jiffies for which the CPU was twiddling thumbs.
///
long IdleJiffies();

///
/// The command name which started a process.
///
std::string Command(int pid);

///
/// Total RAM used by a process (in MB)
///
std::string Ram(int pid);

///
/// User identification number belonging to a process.
///
std::string Uid(int pid);

///
/// User name which belongs to a process.
///
std::string User(int pid);

///
/// Running time of a process, measured in seconds.
///
long int UpTime(int pid);

};  // namespace LinuxParser

#endif