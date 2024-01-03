#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include "linux_parser.h"
#include <vector>

using std::string;
using std::vector;


class Processor {
 public:
  float Utilization();  

 private:
    long prev_total_jiffies{};
    long prev_idle_jiffies{};
    long prev_avtive_jiffies{};
};

#endif