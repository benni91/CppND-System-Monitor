#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    string output, shours, sminutes, sseconds;
    int hours = seconds / 3600;
    if (hours < 10) {
        shours = "0" + to_string(hours);
    }
    else {
        shours = to_string(hours);
    }
    int minutes = (seconds % 3600) / 60;
    if (minutes < 10) {
        sminutes = "0" + to_string(minutes);
    }
    else {
        sminutes = to_string(minutes);
    }
    int iseconds = (seconds % 3600) % 60;
    if (iseconds < 10) {
        sseconds = "0" + to_string(iseconds);
    }
    else {
        sseconds = to_string(iseconds);
    }
    output = shours + ":" + sminutes + ":" + sseconds;
    return output;
    }