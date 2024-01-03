#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Method used to Set process id
void Process::SetPid(int& pid)
{
    this->pid_ = pid;
}

// Return this process's ID
int Process::Pid() const
{ 
    return this->pid_; 
}

// Return this process's CPU utilization
float Process::CpuUtilization() const
{ 
    long process_active_jiffies = LinuxParser::ActiveJiffies(Pid());

    float total_elapsed_time =  LinuxParser::UpTime() - static_cast<float>(LinuxParser::UpTime(Pid())/sysconf(_SC_CLK_TCK)); 

    return static_cast<float>(process_active_jiffies / sysconf(_SC_CLK_TCK)) / total_elapsed_time;
}

// Return the command that generated this process
string Process::Command() 
{ 
    return LinuxParser::Command(Pid()); 
}

// Return this process's memory utilization
string Process::Ram() 
{ 
    return to_string(stol(LinuxParser::Ram(Pid())) / 1000); 
}

// Return the user (name) that generated this process
string Process::User() 
{ 
    return LinuxParser::User(Pid()); 
}

// Return the age of this process (in seconds)
long int Process::UpTime() 
{ 
    return LinuxParser::UpTime(Pid()) / sysconf(_SC_CLK_TCK);
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const 
{ 
    return ( a.CpuUtilization() < this->CpuUtilization()); 
}