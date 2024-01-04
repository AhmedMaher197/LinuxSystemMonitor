#include <string>
#include "format.h"
#include <iomanip>
#include <cmath>

using std::string;
using std::to_string;

// Method used to show time in HH:MM:SS format
string Format::ElapsedTime(long seconds) 
{ 
  double hour = (double) seconds / 3600.0; // seconds in hours
  double hour_int;
  double hour_fra = modf(hour, &hour_int);//split integer and decimal part of hours

  double minutes = hour_fra * 60; // dacimal hours in minutes
  double minutes_int;
  double minutes_fra = modf(minutes, &minutes_int); // split integer and decimal part of minutes

  double seconds_values = minutes_fra * 60; // decimal minutes in seconds
  double seconds_int;
  modf(seconds_values, &seconds_int); // split integer and decimal part of minutes

  std::stringstream str;
  str << std::setfill('0') << std::setw(2) << hour_int << ":" << std::setfill('0') << std::setw(2) <<  minutes_int << ":" << std::setfill('0') << std::setw(2) << seconds_int;
  string time = str.str();
  return time;
}