#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// An example of how to read data from the filesystem
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

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float MemTotal, MemFree;
  bool boMemTotalFound = false, boMemFreeFound = false;
  float memUtil;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);

      // Retrieve the Total Memory Value
      while (!boMemTotalFound && (linestream >> key >> MemTotal)) {
        if (key == "MemTotal:") {
          boMemTotalFound = true;
        }
      }

      // Retrieve the Free Memory Value
      while (!boMemFreeFound && (linestream >> key >> MemFree)) {
        if (key == "MemFree:") {
          boMemFreeFound = true;
        }
      }

      // When we find both the values exit the while loop
      if (boMemFreeFound && boMemTotalFound) break;
    }
  }

  // Calculate the Memory Utilized
  memUtil = (MemTotal - MemFree) / MemTotal;
  return memUtil;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long upTimeValue;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> upTimeValue;
  }
  return upTimeValue;
}

long LinuxParser::Jiffies() {
  // Total Time for Processor in terms of clock ticks/jiffies
  return (ActiveJiffies() + IdleJiffies());
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string pid_str = std::to_string(pid);
  long activeJiffies = 0;
  string strVal;
  long time;
  std::ifstream filestream(kProcDirectory + pid_str + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 1; i <= 17; i++) {
      linestream >> strVal;
      //  utime,     stime,     cutime,    cstime
      if (i == 14 || i == 15 || i == 16 || i == 17) {
        if (strVal != "") {
          time = stol(strVal);
        } else {
          time = 0;
        }
        activeJiffies += time;
      }
    }
  }

  return activeJiffies;
}

long LinuxParser::ActiveJiffies() {
  std::vector<float> cpuActive;
  cpuActive = LinuxParser::CpuUtilization();

  float activeJiffies = cpuActive[0] + cpuActive[1] + cpuActive[2] +
                        cpuActive[5] + cpuActive[6] + cpuActive[7] +
                        cpuActive[8] + cpuActive[9];

  return activeJiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<float> cpuIdle;
  cpuIdle = LinuxParser::CpuUtilization();

  float idleJiffies = cpuIdle[3] + cpuIdle[4];

  return idleJiffies;
}

// Read and return CPU utilization
vector<float> LinuxParser::CpuUtilization() {
  vector<float> cpu_vals{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  string line;
  string key;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> cpu_vals[0] >> cpu_vals[1] >> cpu_vals[2] >>
             cpu_vals[3] >> cpu_vals[4] >> cpu_vals[5] >> cpu_vals[6] >>
             cpu_vals[7] >> cpu_vals[8] >> cpu_vals[9]) {
        if (key == "cpu") {
          return cpu_vals;
        }
      }
    }
  }
  return cpu_vals;
}

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  string pid_str = std::to_string(pid);
  string value;
  std::ifstream filestream(kProcDirectory + pid_str + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string pid_str = std::to_string(pid);
  string key;
  int value;
  string ramMB = "0";

  std::ifstream filestream(kProcDirectory + pid_str + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          // convert the memory from Kb to Mb
          ramMB = to_string(value / 1024);
          return ramMB;
        }
      }
    }
  }
  return ramMB;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string pid_str = std::to_string(pid);
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + pid_str + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);

  string line;
  string x;
  string user;
  string key;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> key) {
        if (key == uid && x == "x") {
          return user;
        }
      }
    }
  }
  return user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  // Get System UpTime in seconds
  long sysUpTime = LinuxParser::UpTime();

  // Process Start Time in seconds
  long procStartTime = LinuxParser::StartTime(pid);

  // Process UpTime in seconds
  return (sysUpTime - procStartTime);
}

// Read and return the process start time in seconds
long LinuxParser::StartTime(int pid) {
  string line;
  string pid_str = std::to_string(pid);
  long hertz = sysconf(_SC_CLK_TCK);
  string strVal;
  long procStartTime = 0;
  int count = 0;
  std::ifstream filestream(kProcDirectory + pid_str + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (count < 22) {
      linestream >> strVal;  // process start time in ticks
      count++;
    }
  }

  if (strVal != "") {
    procStartTime = stol(strVal);
  }

  // Process Start Time in seconds
  return (procStartTime / hertz);
}