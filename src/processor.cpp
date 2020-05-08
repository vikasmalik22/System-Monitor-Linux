#include "processor.h"
#include "linux_parser.h"
#include <vector>

float Processor::Utilization() 
{ 
    float totalTime = LinuxParser::Jiffies();
    float activeTime = LinuxParser::ActiveJiffies() ;

    //# differentiate: actual value minus the previous one
    //total Time since last capture
    float totald = totalTime - m_total;
    
    //total active time since last capture
    float actived = activeTime - m_active;

    float CPU_Percentage = ( actived / totald );

    /* save the current values in the member variables for 
    the next time calculations of these values as previous */
    m_total = totalTime;
    m_active = activeTime;
    
    return CPU_Percentage;        
}