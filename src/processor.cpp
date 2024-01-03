#include "processor.h"

// Return the aggregate CPU utilization
float Processor::Utilization()
{ 
    long current_idle_jiffies = LinuxParser::IdleJiffies();
    long current_active_jiffies = LinuxParser::ActiveJiffies();
    long current_total_jiffies = current_idle_jiffies + current_active_jiffies;

    long totald = current_total_jiffies - prev_total_jiffies;
    long idled =  current_idle_jiffies - prev_idle_jiffies;

    prev_avtive_jiffies = current_active_jiffies;
    prev_idle_jiffies = current_idle_jiffies;
    prev_total_jiffies = current_total_jiffies;

   return static_cast<float>(totald - idled) / totald; 
}