#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { return m_pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() 
{ 
    // Process Total Time in Seconds
    long procTotalTime = LinuxParser::UpTime(m_pid);

    // Process Active Time in Seconds
    long procActiveTime = LinuxParser::ActiveJiffies(m_pid) / sysconf(_SC_CLK_TCK);

    float proc_CpuUtil = (float)procActiveTime / (float)procTotalTime;

    return proc_CpuUtil;
}

// Return the command that generated this process
string Process::Command() 
{ 
    return LinuxParser::Command(m_pid); 
}

// Return this process's memory utilization
string Process::Ram() 
{ 
    return LinuxParser::Ram(m_pid); 
}

// Return the user (name) that generated this process
string Process::User() 
{ 
    return LinuxParser::User(m_pid); 
}

// Return the age of this process (in seconds)
long int Process::UpTime() 
{ 
    return LinuxParser::UpTime(m_pid); 
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const 
{ 
    return ( a.m_pCpuUtil < this->m_pCpuUtil );
}