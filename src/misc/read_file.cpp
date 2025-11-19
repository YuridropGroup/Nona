// Standard library imports.

#include <fstream>
#include <string>
#include <vector>
#include <chrono>

std::vector<uint8_t> read_file(const std::string &path) {

    /*
    Read the bit stream of a file
    */

    std::ifstream file(path , std::ios::binary);
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file) , std::istreambuf_iterator<char>());

}