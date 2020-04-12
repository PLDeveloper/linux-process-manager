#ifndef PROCESS_H
#define PROCESS_H

#include <string>
using std::string;

/*
Basic class for Process representation
*/
class Process {
 public:
  int Pid();                             
  string User(); 
  string Command();                  
  float CpuUtilization();                 
  string Ram() const;                      
  long int UpTime();                      
  bool operator<(Process const& a) const; 

  Process(int pid) : pid_(pid) {};

 private:
  int pid_;
  string user_ = "";
  string command_ = ""; 
};

#endif