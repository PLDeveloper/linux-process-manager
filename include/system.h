#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

using std::string;
using std::vector;

class System {
 public:
  Processor& Cpu();                  
  vector<Process>& Processes(); 
  float MemoryUtilization();         
  long UpTime();                     
  int TotalProcesses();              
  int RunningProcesses();            
  string Kernel();              
  string OperatingSystem();     

  System();

 private:
  Processor cpu_ = {};
  vector<Process> processes_ = {};
  string kernel_ = "";
  string operating_system_ = "";
};

#endif