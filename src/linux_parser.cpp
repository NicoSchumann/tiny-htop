#include "linux_parser.h"

#include <unistd.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
// Maybe you need to replace by:
// namespace fs = std::experimental::filesystem;
// if your standard library is not yet C++17 compatible.
namespace fs = std::filesystem;

using std::stof;
using std::string;
using std::to_string;
using std::vector;

/// DONE: An example of how to read data from the filesystem.
string LinuxParser::OperatingSystem() {
  string line;
  string key, kKey("PRETTY_NAME");
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == kKey) {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

/// DONE: An example of how to read data from the filesystem.
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

///
/// Returns all Pids, found as file names.
///
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  for (const auto& file : fs::directory_iterator(kProcDirectory)) {
    if (file.is_directory()) {
      std::string filename = file.path().stem().string();
      // Is every character of the name a digit?
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  return pids;
}

///
/// Reads the memory utilizaion in fraction of total memory (0.0f-1.0f).
///
float LinuxParser::MemoryUtilization() {
  std::string tmp;
  float memtotal{0.0f}, memfree{0.0f};
  std::ifstream ifstr(kProcDirectory + kMeminfoFilename);
  if (!(ifstr >> tmp >> memtotal >> tmp >> tmp >> memfree)) {
    throw 1;
  }
  return (memtotal - memfree) / memtotal;
}

///
/// Returns the system uptime in seconds.
///
long LinuxParser::UpTime() {
  double uptime{0.0};
  std::ifstream ifs(kProcDirectory + kUptimeFilename);
  if (!(ifs >> uptime)) {
    throw 2;
  }
  return static_cast<long>(uptime);
}

///
/// Reads and returns the number of jiffies for the system.
///
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

///
/// Reads and returns the number of active jiffies for a PID.
///
long LinuxParser::ActiveJiffies(int pid) {
  long u_time, s_time, cu_time, cs_time;
  std::string tmp, line;
  std::ifstream ifs(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (!ifs)
    throw std::runtime_error(
        "LinuxParser::ActiveJiffies(int pid):ifs couln't opened.");
  std::getline(ifs, line);
  ifs.close();
  std::istringstream iss(line);

  /// PROBLEM: The vals may be wrong (too small) at my machine!
  ///
  for (int i = 1; i < 14; ++i) {
    iss >> tmp;
  }
  iss >> u_time >> s_time >> cu_time >> cs_time;  // vals 14, 15, 16, 17
  return u_time + s_time + cu_time + cs_time;
}

///
/// Reads and returns the number of active jiffies for the system.
///
long LinuxParser::ActiveJiffies() {
  long active_jiffies{0};
  std::string line;
  std::ifstream ifs(kProcDirectory + kStatFilename);

  while (std::getline(ifs, line) && line.substr(0, 3) == std::string("cpu")) {
    if (!std::isdigit(line[3])) continue;

    std::istringstream iss(line.substr(6));
    long tmp;
    std::vector<long> cpu_states;
    while (iss >> tmp) {
      cpu_states.push_back(tmp);
    }
    using namespace LinuxParser;
    for (std::size_t state{0}; state < cpu_states.size(); ++state) {
      switch (state) {
        case kUser_:
        case kNice_:
        case kSystem_:
        case kIRQ_:
        case kSoftIRQ_:
        case kSteal_:
        case kGuest_:
        case kGuestNice_:
          active_jiffies += cpu_states[state];
      }
    }
  }
  return active_jiffies;
}

///
/// Read and return the number of idle jiffies for the system.
///
long LinuxParser::IdleJiffies() {
  long idle_jiffies{0};
  std::string line;
  std::ifstream ifs(kProcDirectory + kStatFilename);

  while (std::getline(ifs, line) && line.substr(0, 3) == std::string("cpu")) {
    if (!std::isdigit(line[3])) continue;

    std::istringstream iss(line.substr(6));
    long tmp;
    std::vector<long> cpu_states;
    while (iss >> tmp) {
      cpu_states.push_back(tmp);
    }
    for (std::size_t state{0}; state < cpu_states.size(); ++state) {
      switch (state) {
        case LinuxParser::kIdle_:
        case LinuxParser::kSteal_:
          idle_jiffies += cpu_states[state];
      }
    }
  }
  return idle_jiffies;
}

///
/// Reads and returns CPU utilization (the above lines of the /proc/stat file).
///
vector<string> LinuxParser::CpuUtilization() {
  std::string line, kCpu("cpu");
  std::vector<std::string> result;

  std::ifstream ifs(kProcDirectory + kStatFilename);
  while (std::getline(ifs, line)) {
    if (line.substr(0, 3) == kCpu) {
      result.push_back(line.substr(5));
    } else {
      break;  // There are no more lines needed to read.
    }
  }
  return result;
}

///
/// Reads and returns the total number of processes.
///
int LinuxParser::TotalProcesses() {
  std::string line, kProcesses("processes");
  int total_processes{0};
  std::ifstream ifs(kProcDirectory + kStatFilename);
  while (std::getline(ifs, line)) {
    if (line.substr(0, 9) == kProcesses) {
      std::istringstream iss(line.substr(10));
      if (!(iss >> total_processes)) break;
      return total_processes;
    }
  }
  // If comes to that line, an error happend.
  throw 4;
}

///
/// Reads and returns the number of running processes.
///
int LinuxParser::RunningProcesses() {
  int running_processes;
  std::string line, kProcs_running("procs_running");
  std::ifstream ifs(kProcDirectory + kStatFilename);
  while (std::getline(ifs, line)) {
    if (line.substr(0, 13) == kProcs_running) {
      std::istringstream iss(line.substr(14));
      if (!(iss >> running_processes)) break;
      return running_processes;
    }
  }
  // If comes to that line, an error happend.
  throw 5;
}

///
/// Reads and returns the command associated with a process.
///
string LinuxParser::Command(int pid) {
  std::ifstream ifs(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (!ifs) throw std::runtime_error("LinuxParser::Command()");
  std::string line;
  std::getline(ifs, line);
  std::istringstream iss(line);
  iss >> line;
  fs::path path(line);
  return path.filename().string();
}

///
/// Reads and returns the memory used by a process (in MB).
///
string LinuxParser::Ram(int pid) {
  std::ifstream ifs(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (!ifs) throw std::runtime_error("LinuxParser::Ram()");
  std::string tmp;
  long vsize;  // float?
  for (int i = 1; i < 23; ++i) {
    ifs >> tmp;
  }
  ifs >> vsize;          //  bytes
  vsize /= 1024 * 1024;  // MB
  return tmp = std::to_string(vsize);
  /*
  auto decpoint = tmp.find('.');
  return tmp.substr(0,decpoint+3);
  */
}

///
/// Reads and returns the user ID associated with a process.
///
string LinuxParser::Uid(int pid) {
  std::ifstream ifs(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (!ifs) throw std::runtime_error("LinuxParser::Uid(int pid)");
  std::string line, uid, kUid("Uid:");
  while (std::getline(ifs, line)) {
    if (line.substr(0, 4) == kUid) {
      std::istringstream iss(line.substr(5));
      iss >> uid;
      break;
    }
  }
  return uid;
}

///
/// Read and return the user associated with a process.
///
string LinuxParser::User(int pid) {
  std::string uid = LinuxParser::Uid(pid);
  std::string line, user, tmp;
  std::ifstream ifs(kPasswordPath);
  if (!ifs) throw std::runtime_error("LinuxParser::Uid(int pid)");
  while (std::getline(ifs, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream iss(line);
    iss >> user >> tmp;
    if (tmp == std::string("x")) iss >> tmp;
    iss >> tmp;
    if (tmp == uid) {
      break;
    }
  }
  return user;
}

///
/// Reads and returns the uptime of a process.
/// Measured in seconds.
///
long LinuxParser::UpTime(int pid) {
  static const int hertz = sysconf(_SC_CLK_TCK);
  long process_start_time;
  std::string line, tmp;
  std::ifstream ifs(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (!ifs) {
    throw std::runtime_error("LinuxParser::UpTime(int pid):ifs not open.");
  }
  std::getline(ifs, line);
  std::istringstream iss(line);
  for (int i = 1; i < 22; ++i) {
    iss >> tmp;
  }
  if (!(iss >> process_start_time)) {
    throw std::runtime_error(
        "LinuxParser::UpTime(int pid):Couldn't read val 22.");  // measured in
                                                                // clock times
  }
  process_start_time /= hertz;  // convert to seconds
  long system_up_time = LinuxParser::UpTime();
  return system_up_time - process_start_time;
}
