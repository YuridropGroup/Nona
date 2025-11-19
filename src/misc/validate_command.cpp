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

bool validate(std::string command) {

    /*
    Validate the user entered command.
    */

    std::ifstream command_dictionary("config/commands.json");
    if (!command_dictionary) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ validate_command.cpp ] Unable to open commands.json." << std::endl;
    }

    json commands;
    try {
        command_dictionary >> commands;
    }
    catch (std::exception error) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ validate_command.cpp ] Unable to open parse client.json: " << error.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Sanity check client.json.

    if (!commands.contains("basic")) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ validate_command.cpp ] client.json does not contain the key \"basic\"." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    else if (!commands.contains("discord")) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ validate_command.cpp ] client.json does not contain the key \"discord\"." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (commands["basic"].contains(command)) {
        return true;
    }
    else if (commands["discord"].contains(command)) {
        return true;
    }
    else {
        return false;
    }

}