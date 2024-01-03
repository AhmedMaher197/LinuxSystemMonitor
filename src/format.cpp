#include <string>
#include "format.h"

using std::string;
using std::to_string;

// Method used to show time in HH:MM:SS format
string Format::ElapsedTime(long seconds) 
{ 
    uint64_t hours = seconds / (60 * 60);
    seconds = seconds % (60 * 60);

    uint64_t minutes = seconds / 60;
    seconds = seconds % 60;
    
    return to_string(hours) + ':' + to_string(minutes) + ':' + to_string(seconds); 

}