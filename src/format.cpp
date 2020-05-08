#include <string>

#include "format.h"

using std::string;

string TimeValToString(int Time)
{
    std::string strTime;
    
    if( Time >= 0 && Time <= 9)
    {
        strTime = '0' + std::to_string(Time);
    }
    else    
    {
        strTime = std::to_string(Time);
    }

    return strTime;
}


// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long times) 
{ 
    int hour = times / 3600;
    int mins = (times % 3600) / 60;
    int secs = (times % 3600) % 60;
    
    string HH = TimeValToString(hour);
    string MM = TimeValToString(mins);
    string SS = TimeValToString(secs);

    return (HH + ":" + MM + ":" + SS);
}




