#include <unistd.h>
#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() {
  vector<int> pids = LinuxParser::Pids();
  for (int p : pids) {
    processes_.push_back(Process(p));
  }
  // Order process list high-to-low by RAM usage
  std::sort(processes_.begin(), processes_.end());
  std::reverse(processes_.begin(), processes_.end());
}

Processor& System::Cpu() { 
  return cpu_; 
}

vector<Process>& System::Processes() { 
  return processes_; 
}

std::string System::Kernel() { 
  if (kernel_.empty()) {
    kernel_ = LinuxParser::Kernel();
  }
  return kernel_; 
}

std::string System::OperatingSystem() { 
  if (operating_system_.empty()) {
    operating_system_ = LinuxParser::OperatingSystem();
  }
  return operating_system_;
}

float System::MemoryUtilization() { 
  return LinuxParser::MemoryUtilization(); 
}

int System::RunningProcesses() { 
  return LinuxParser::RunningProcesses(); 
}

int System::TotalProcesses() { 
  return LinuxParser::TotalProcesses(); 
}

long int System::UpTime() { 
  return LinuxParser::UpTime(); 
}