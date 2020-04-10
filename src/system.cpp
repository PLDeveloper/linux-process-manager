#include <unistd.h>
#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

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
  return LinuxParser::Kernel(); 
}

float System::MemoryUtilization() { 
  return LinuxParser::MemoryUtilization(); 
}

std::string System::OperatingSystem() { 
  return LinuxParser::OperatingSystem(); 
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