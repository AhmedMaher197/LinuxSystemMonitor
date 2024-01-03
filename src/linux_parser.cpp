#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// Read Operating System name
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Read kernal name in the system
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  uint32_t total_memory{};
  uint32_t free_memory{};
  float total_used_memory{};
  string key{};
  string line{};

  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  if(stream.is_open())
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "MemTotal:") {
          linestream >> total_memory;
        }
        if (key == "MemFree:") {
          linestream >> free_memory;
        }
      }
    }
  }

  total_used_memory = static_cast<float>(total_memory - free_memory) / total_memory;

  return total_used_memory; 
}

// Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  long uptime;
  string line{};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return uptime;
  }
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{ 
  return ActiveJiffies() + IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) 
{ 
  uint8_t count{};
  string line{};
  string data{};
  long total_active_jiffies{};

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);

  if(stream.is_open())
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);

      while(linestream >> data)
      {
        count++;
        if (count >= 14 && count <= 17) {
          total_active_jiffies += stol(data);
        }
        else if(count > 17)
        {
          return total_active_jiffies;
        }
      }
    }
  }
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() 
{ 
  vector<string> jiffies = CpuUtilization();
  
  return stol(jiffies[kUser_]) + stol(jiffies[kNice_]) + stol(jiffies[kSystem_]) + stol(jiffies[kIRQ_]) + stol(jiffies[kSoftIRQ_]) + stol(jiffies[kSteal_]); 
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{ 
  vector<string> jiffies = CpuUtilization();

  return stol(jiffies[kIdle_]) + stol(jiffies[kIOwait_]); 
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() 
{ 
  string line{};
  string key{};
  string jiffy{};
  vector<string> jiffies{};

  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if(stream.is_open())
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu")
        {
          while(linestream >> jiffy)
            jiffies.emplace_back(jiffy);
          
          return jiffies;
        }
      }
    }
  }
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
  string line{};
  string key{};
  int value{};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open())
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "processes") {
          linestream >> value;
          return value;
        }
      }
    }
  }
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{  
  string line{};
  string key{};
  int value{};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open())
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "procs_running") {
          linestream >> value;
          return value;
        }
      }
    }
  }
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{ 
  string command{};
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);

  if(stream.is_open())
  {
    if(std::getline(stream, command)) 
    {
      return command;
    }
  } 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) 
{ 
  string line{};
  string key{};
  string memory_used{};
  std::ifstream stream(kProcDirectory + to_string(pid) +kStatusFilename);

  if(stream.is_open())
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "VmSize:") {
          linestream >> memory_used;
          return memory_used;
        }
      }
    }
  } 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{ 
  string line{};
  string key{};
  string uid{};

  std::ifstream stream(kProcDirectory + to_string(pid) +kStatusFilename);

  if(stream.is_open())
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "Uid:") {
          linestream >> uid;
          return uid;
        }
      }
    }
  } 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) 
{ 
  string line{};
  string key{};
  string user{};
  string uid{};
  std::ifstream stream(kPasswordPath);

  if(stream.is_open())
  {
    while (std::getline(stream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user;
      linestream >> uid >> uid;
      if (uid == Uid(pid)) {
        return user;
      }
    }
  } 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) 
{ 
  uint8_t count{};
  string line{};
  string up_time{};

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);

  if(stream.is_open())
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);

      while(linestream >> up_time)
      {
        count++;
        if (count == 22)
        {
          return stol(up_time);
        }
      }
    }
  }
}
