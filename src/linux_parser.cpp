#include <dirent.h>
#include <math.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

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

// Calculates the system memory usage from data in /proc/meminfo using the same approach as htop
// https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
float LinuxParser::MemoryUtilization() { 
  float mem_total, mem_free, total_usage; 
  string line, name, value, units;
  std::ifstream file(kProcDirectory + kMeminfoFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {  
      std::istringstream lstream(line);
      lstream >> name >> value >> units; 
      if (!value.empty()) {
        if (name == "MemTotal:") {
          mem_total = stof(value);
        } else if (name == "MemFree:") {
          mem_free = stof(value);
        } else {
          break;
        }
      }
    }
  }
  total_usage = (mem_total - mem_free) / mem_total;
  return total_usage; 
}

// Returns the uptime of the entire system from /proc/uptime 
long LinuxParser::UpTime() { 
  string line; 
  float uptime = 0.0; 
  std::ifstream file(kProcDirectory + kUptimeFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream lstream(line);
    lstream >> uptime; 
  }
  return long(uptime);  
}

// Read and returns the set of numbers in /proc/stat used to compute the CPU utilization
// Called in Processor::Utilization() to perform the actual calculation
vector<string> LinuxParser::CpuUtilization() { 
  string line, value;
  vector<string> values;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream lstream(line);
    // Toss out the leading 'cpu' word...
    lstream >> value; 
    // ...then collect the rest of the relevant values
    while (lstream >> value) {
      values.push_back(value);
    }
  }
  return values; 
}

// Reads and returns the total number of processes stored in /proc/stat
int LinuxParser::TotalProcesses() { 
  string line, key;
  int total_processes = 0; 
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream lstream(line);
      lstream >> key;
      if (key == "processes") {
        lstream >> total_processes;
        break;
      }
    }
  }
  return total_processes; 
}

// Read and return the number of running processes from /proc/stat
int LinuxParser::RunningProcesses() { 
  string line, key;
  int procs_running = 0;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream lstream(line);
      if (lstream >> key && key == "procs_running") {
        lstream >> procs_running;
        break;
      }
    }
  }
  return procs_running; 
}

// Read and return the command associated with a process 
string LinuxParser::Command(int pid) { 
  string fname = "/proc/" + to_string(pid) + "/cmdline";
  string line = "";
  std::ifstream file(fname);
  if (file.is_open()) {
    std::getline(file, line);
  }
  return line;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line, key;
  string fname = "/proc/" + to_string(pid) + "/status"
  string uid = "";
  std::ifstream file(fname);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream lstream(line);
      if (lstream >> key && key == "Uid:") {
        lstream >> uid;
        break;
      }
    }
  }
  return uid; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }