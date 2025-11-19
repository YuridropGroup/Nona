// Standard library imports.

#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <future>
#include <vector>
#include <fstream>

// External library imports.

#include "dpp/dpp.h"
#include "nlohmann/json.hpp"

// Custom module imports.

#include "client.h"
#include "events/on_ready.h"
#include "interface/loadGUI.h"
#include "interface/loadCLI.h"
#include "misc/colours.h"
#include "misc/time.h"
#include "misc/help.h"
#include "redis/precaching.h"
#include "redis/rate_limiter.h"

using json = nlohmann::json;

dpp::snowflake guild_id = 0;

int main(int argc , char* argv[]) {

    /*
    The entry function for Nona.
    */

    // Handle command-line flag arguments.

    const char *mode = "c";

    if (argc > 1) {
        std::string_view argument = argv[1];
        if (argument == "--help" || argument == "-h") {
            if (argc > 2) {
                help(argv[2]);
            }
            else {
                help();
            }
            return 0;
        }
        else if (argument == "--gui" || argument == "-g") {
            mode = (char*)"g";
        }
        else if (argument == "--cli" || argument == "-c") {
            mode = (char*)"c";
        }
        else {
            std::cout << MAG << get_current_time() << RED << "[ ERROR ] An invalid argument was passed: " << argument << std::endl;
            return 1;
        }
    }
    else {
        mode = "c";
    }

    // Load the client.json configuration file.

    std::ifstream client("config/client.json" , std::ios::binary);
    if (!client) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] Unable to open client.json." << std::endl;
        return 1;
    }

    json config;

    try {
        client >> config;
    }
    catch (const std::exception &error) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] Unable to open parse client.json: " << error.what() << std::endl;
        return 1;
    }

    // Sanity check client.json.

    if (!config.contains("Nona")) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] client.json does not contain the key \"Nona\"." << std::endl;
        return 1;
    }
    if (!config["Nona"].contains("Discord")) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] client.json does not contain the key \"Discord\"." << std::endl;
        return 1;
    }
    if (!config["Nona"]["Discord"].contains("Tokens")) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] client.json does not contain the key \"Tokens\"." << std::endl;
        return 1;
    }
    if (!config["Nona"]["Discord"]["Tokens"].is_array()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] The key \"Tokens\" in client.json must be an array." << std::endl;
        return 1;
    }
    for (auto item : config["Nona"]["Discord"]["Tokens"]) {
        if (!item.is_string()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] The token \"" << item << "\" in the key \"Tokens\" in client.json must be a string." << std::endl;
            return 1;
        }
    }
    std::vector<std::string> tokens = config["Nona"]["Discord"]["Tokens"];
    if (tokens.size() > 1) {
        std::string user_token = config["Nona"]["Discord"]["userToken"];
        if (user_token.empty()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] To use more than bot, a user token (with guild permissions) must be provided in client.json." << std::endl;
            return 1;
        }
    }
    else if (tokens.size() == 0) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] No token was provided. Please provide one in client.json and try again." << std::endl;
        return 1;
    }

    std::cout << MAG << "[ " << get_current_time() << " ]" << WHT << "Nona's token(s) has been loaded." << std::endl;

    // Detect operating system, if Windows or undetected then hard-select CLI.

    std::string operating_system;

    #ifdef _WIN64
        mode = "c";
        operating_system = "windows";
        std::cout << MAG << "[ " << get_current_time() << " ] " << WHT << "Windows detected: hardcoded CLI mode." << std::endl;
        std::system("cls");
        std::system("title Nona");
    #elif __linux__
        std::cout << MAG << "[ " << get_current_time() << " ] " << WHT << "Operating system detected: Linux." << std::endl;
        operating_system = "linux";
        std::system("clear");
    #else
        std::cout << MAG << "[ " << get_current_time() << " ] " << WHT << "[ ERROR ][ index.cpp ] Unable to detect operating system." << std::endl;
        operating_system = "linux";
        std::system("clear");
        mode = "c";
    #endif

    try {

        std::vector<std::thread> botThreads;
        int total_bots = tokens.size();
        for (size_t i = 0; i < tokens.size(); ++i) {
            try {
                botThreads.emplace_back([i , &tokens , total_bots] {
                    std::string token = tokens[i];
                    dpp::cluster Nona(token);
                    auto limiter = std::make_shared<RateLimiter>(49);
                    loadEvents(Nona , i , total_bots , limiter);
                    loadCogs(Nona); // Potentially redundant?
                    Nona.start(dpp::st_wait);
                });
            }
            catch (const std::exception &error) {
                std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] Unable to initialise client " << i << ": " << error.what() << std::endl;
            }
        }

        // Detach every bot's private thread.

        for (auto &thread : botThreads) {
            thread.detach();
        }

        // Wait for all bots to come online.

        {
            std::unique_lock<std::mutex> lock(ready_mutex);
            all_ready_cv.wait(lock , [total_bots] {
                return ready_bots == total_bots;
            });
        }

        if (ready_bots == 0) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << WHT << "[ ERROR ][ index.cpp ] No bots were loaded, please try a VPN or new bot tokens for a different account." << std::endl;
            return 1;
        }
        else {
            std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "All bots are online." << std::endl;
        }

        // Ask user for guild ID

        while (true) {
            std::string guild_id_str;
            std::cout << MAG << "[ " << get_current_time() << " ] " << WHT << "Please enter the ID of the guild you want to attack: ";
            std::cin >> guild_id_str;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
            try {
                guild_id = std::stoull(guild_id_str);
                break;
            }
            catch (...) {
                std::cout << MAG << "[ " << get_current_time() << " ] " << WHT << "[ ERROR ][ index.cpp ] An invalid guild ID was entered, please try again." << std::endl;
                return 1;
            }
        }

        // Precache using first bot

        std::thread precache_thread([&tokens] {
            dpp::cluster precache_bot(tokens[0]);
            precache_bot.on_ready([&precache_bot](const dpp::ready_t& event) {
                precache_objects(precache_bot , guild_id);
            });
            precache_bot.start(dpp::st_wait);
        });
        precache_thread.detach();

    }
    catch (const std::exception &error) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ index.cpp ] An error occurred initialising the clients: " << error.what() << std::endl;
    }

    // Manage mode selection.

    // c = Command Line Interface | ( CLI )
    // g = Graphical User Interface | ( GUI )

    if (!mode) mode = "g";
    (strcmp(mode , "g") == 0) ? loadGUI() : loadCLI(operating_system , guild_id);

}
