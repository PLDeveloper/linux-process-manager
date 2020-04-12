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
  string fname = "/proc/" + to_string(pid) + "/status";
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

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string fname = "/proc/" + to_string(pid) + "/status";
  string line, key;
  string memory = ""; 
  std::ifstream file(fname);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream lstream(line);
      if (lstream >> key && key == "VmSize:") {
        lstream >> memory; 
        break;
      }
    }
  }
  return memory;
}

// Gets the user name associated with a particular process
string LinuxParser::User(int pid) { 
  string line, toss, uid;
  string user = "";
  // Get the uid associated with the target process
  int target = std::stoi(LinuxParser::Uid(pid));
  // Map the target uid to it's user name 
  std::ifstream file(kPasswordPath);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lstream(line);
      lstream >> user >> toss >> uid;
      if (!uid.empty() && std::stoi(uid) == target) {
        break;
      }
    }
  }
  return user; 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  long systime = 0; 
  string line, value;
  string fname = "/proc/" + to_string(pid) + "/stat";
  std::ifstream file(fname);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream lstream(line);
    // The 22nd number in the stream is the value we want
    for (int i = 0; i < 22; i++) { 
      lstream >> value;
    }
    if (!value.empty()) {
      systime = std::stol(value) / sysconf(_SC_CLK_TCK);
    }
  }
  return systime; 
}

float LinuxParser::ProcessCPU(int pid) {
  string fname = "/proc/" + to_string(pid) + "/stat";
  string line, value; 
  float uptime, utime, stime, starttime, hertz;
  uptime = LinuxParser::UpTime();
  std::ifstream file(fname);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream lstream(line);
    int i = 0; 
    while (lstream >> value && !value.empty()) {
      if (i == 13) {
        utime = std::stof(value);
      } else if (i == 14) {
        stime = std::stof(value);
      } else if (i == 21) {
        starttime = std::stof(value);
      }
      i++; 
    }
  }
  hertz = sysconf(_SC_CLK_TCK);
  float total_time, seconds, cpu_usage;
  total_time = utime + stime;
  seconds = uptime - (starttime / hertz);
  cpu_usage = 100*(total_time / hertz) / seconds;
  return cpu_usage;
}