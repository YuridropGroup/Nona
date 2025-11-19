// Standard library imports.

#include <iostream>
#include <string>

std::string lowerString(std::string &command)
{

    std::string result = command;

    for (char &character : result) {
        character = tolower(character);
    }
    
    return result;
}