// Standard library imports.

#include <iostream>
#include <string>
#include <fstream>

// External library imports.

#include "nlohmann/json.hpp"

// Custom module imports.

#include "colours.h"
#include "time.h"

using json = nlohmann::json;

void help(std::string command = "default") {

    /*
    The help command (and flag).
    */

    std::ifstream command_dictionary("config/commands.json");
    if (!command_dictionary) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "Unable to open commands.json." << std::endl;
    }

    json commands;
    command_dictionary >> commands;

    if (command == "default") {
        std::cout << MAG << "[ " << get_current_time() << " ]" << WHT << " Here are the commands:" << std::endl;
        if (commands.contains("basic") && commands["basic"].is_object()) { 
            for (auto &[command_name , command_info] : commands["basic"].items()) {
                std::string description = command_info.value("description" , "No Description.");
                std::string usage       = command_info.value("usage" , "No usage.");
                std::cout << MAG << "[ " << get_current_time() << " ] " << WHT 
                << command_name << "\n               Description: " << description 
                << "\n               Usage:       " << usage << std::endl;
            }
        }
        if (commands.contains("discord") && commands["discord"].is_object()) { 
            for (auto &[command_name , command_info] : commands["discord"].items()) {
                std::string description = command_info.value("description" , "No Description.");
                std::string usage       = command_info.value("usage" , "No usage.");
                std::cout << MAG << "[ " << get_current_time() << " ] " << WHT 
                << command_name << "\n               Description: " << description 
                << "\n               Usage:       " << usage << std::endl;
            }
        }
        else {
            std::cout << MAG << "[ " << get_current_time() << " ]" << RED << "Invalid JSON data structure." << std::endl;
        }
    }
    else if (commands["basic"].contains(command)) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << WHT << command << "\n    Description: " << commands["basic"][command]["description"] << "\n    Usage:" << commands["basic"][command]["usage"] << std::endl;
    }
    else if (commands["discord"].contains(command)) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << WHT << command << "\n    Description: " << commands["discord"][command]["description"] << "\n    Usage:" << commands["discord"][command]["usage"] << std::endl;
    }
    else {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << command << " is not a registered command." << std::endl;
    }

}