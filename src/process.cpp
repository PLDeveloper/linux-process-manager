#include <math.h>
#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { 
  return pid_; 
}

float Process::CpuUtilization() { 
  return LinuxParser::ProcessCPU(pid_); 
}

string Process::Command() { 
  return LinuxParser::Command(pid_); 
}

// Retrieves process memory in KB & converts to MB
string Process::Ram() const { 
  string ram = LinuxParser::Ram(pid_);
  if (!ram.empty()) {
    float mb = std::stof(ram) / 1000;
    ram = std::to_string(int(mb));
  }
  return ram; 
}

string Process::User() { 
  return LinuxParser::User(pid_); 
}

long int Process::UpTime() { 
  return LinuxParser::UpTime(pid_); 
}

// Use RAM usage as the basis for comparing two processes 
bool Process::operator<(Process const& a) const { 
  return std::stoi(this->Ram()) < std::stoi(a.Ram()); 
}