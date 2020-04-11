#include <cassert>
#include <string>
#include "processor.h"
#include "linux_parser.h"

using std::string;
using std::stof;

// Returns the aggregate CPU utilization (i.e. summed across cores)
float Processor::Utilization() { 
  auto values = LinuxParser::CpuUtilization(); 
  for (string v : values) {
    if (v.empty()) {
      v = "0";
    }
  }

  float user, nice, system, idle, iowait, irq, softirq, steal;
  user = stof(values[0]); 
  nice = stof(values[1]); 
  system = stof(values[2]);
  idle = stof(values[3]);
  iowait = stof(values[4]);
  irq = stof(values[5]);
  softirq = stof(values[6]);
  steal = stof(values[7]);

  float idle_time = idle + iowait;
  float non_idle_time = user + nice + system + irq + softirq + steal;
  float total = idle_time + non_idle_time;
  assert(total != 0);
  return non_idle_time / total;
}