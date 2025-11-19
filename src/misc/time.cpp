// Standard library imports.

#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string get_current_time() {

    /*
    Return the current time to display as prefixes for the console.
    */

    auto now = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(now);
    tm* localTime = localtime(&currentTime);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%H:%M:%S");
    return oss.str();

}