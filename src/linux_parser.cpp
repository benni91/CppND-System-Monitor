#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"
#include "format.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
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
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float Total = findValueByKey<float>(filterMemTotalString, kMeminfoFilename);// "/proc/memInfo"
  float Free = findValueByKey<float>(filterMemFreeString, kMeminfoFilename);
  return (Total - Free) / Total;
  }

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
  }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> output; 
  string line;
  string key;
  string value;
  //int user, nice, system;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
      std::istringstream linestream(line);
      while (linestream >> value) {
        output.emplace_back(value);
      }
    
  }
  return output; 
  }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  float value = findValueByKey<float>(filterProcesses, kStatFilename);// "/proc/memInfo"
          return value;
  }

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  float value = findValueByKey<float>(filterRunningProcesses, kStatFilename);// "/proc/memInfo"
          return value;
  }

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  return std::string(getValueOfFile<std::string>(std::to_string(pid) + kCmdlineFilename));
  }

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string value = findValueByKey<string>(filterProcMem, to_string(pid)+kStatusFilename);// "/proc/[pid]/status"
  return value.substr(0,size(value)-3);
  }

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string value = findValueByKey<string>(filterUID, to_string(pid)+kStatusFilename);// "/proc/[pid]/status"
          return value;
   }

// Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line;
  string pwd;
  string value;
  string name;
  std::ifstream filestream(kPasswordPath);
   if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> pwd >> value) {
        if (value == uid) {
          return name;
          
        }
      }
    }
  }
   return string(); 
   }

// Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  string key;
  string value;
  string name;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
   if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      int i = 0;
      while (linestream >> value) {
        if (i == 23) {
          //std::cout << value << "\n";
          return std::stol(value) / sysconf(_SC_CLK_TCK);
          
        }
        i++;
      }
    }
  }
  return 0; 
  }

  float LinuxParser::CpuUtilization(int pid) {
    string line, value;
    long systemUpTime = LinuxParser::UpTime();
    long utime, stime, cutime, cstime, starttime, total_time;
    vector<string> data;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
    if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      for (int i = 0; i < 22; i++) {
        linestream >> value;
        data.push_back(value);
      }
    }
    utime = stol(data[13]);
    stime = stol(data[14]);
    //cutime = stol(data[15]);
    //cstime = stol(data[16]);
    starttime = stol(data[21]);
    total_time = utime + stime;
    //total_time = total_time + cutime + cstime;
    float seconds = (float) (systemUpTime - (starttime / sysconf(_SC_CLK_TCK)));
    float cpu_usage = ((total_time / sysconf(_SC_CLK_TCK))) / seconds;
    return cpu_usage;
  }
