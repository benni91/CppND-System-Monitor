#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    auto input = LinuxParser::CpuUtilization();
    int nonIdle = stoi(input[1])+ stoi(input[2])+stoi(input[3])+stoi(input[6])+stoi(input[7]) +stoi(input[8]);
    int idle = stoi(input[4]) +stoi(input[5]);
    return float(nonIdle) / float(idle + nonIdle); 
    }