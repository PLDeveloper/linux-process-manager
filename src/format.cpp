#include <string>
#include <stdio.h>
#include "format.h"

using std::string;

// Input: 
//  Total number of seconds
// Output: 
//  HH:MM:SS
// Notes: 
//  Using integer division 
//    seconds = 3600 * HH + 60 * MM + SS 
//    -> seconds / 3600 = HH 
//    -> seconds % 3600 = 60 * MM + SS 

string Format::ElapsedTime(long seconds) { 
  int h, m, s;

  h = seconds / 3600; 
  seconds = seconds % 3600;

  m = seconds / 60;
  seconds = seconds % 60; 

  s = seconds;

  char buf[10];
  sprintf(buf, "%02d:%02d:%02d", h, m, s);

  return string(buf); 
}