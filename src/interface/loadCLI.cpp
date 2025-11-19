// Standard library imports.

#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <future>
#include <random>
#include <functional>
#include <unordered_map>

// External library imports.

#include "dpp/dpp.h"
#include "nlohmann/json.hpp"

// Custom module imports.

#include "../index.h"
#include "../misc/colours.h"
#include "../misc/time.h"
#include "../misc/help.h"
#include "../misc/lower_string.h"
#include "../misc/validate_command.h"
#include "../events/on_ready.h"
#include "../redis/precaching.h"
#include "command_worker.h"

using json = nlohmann::json;

const json &get_splashes_config() {

    /*
    Static makes this only be loaded once.
    */

    static json splashes_config = [] {

        // Load splashes.json

        std::ifstream unloaded_splashes("config/splashes.json");
        if (!unloaded_splashes) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "Unable to open splashes.json." << std::endl;
            return json{};
        }

        json splashes;
        unloaded_splashes >> splashes;

        // Fetch channel names.

        if (!splashes.contains("splashes") || !splashes["splashes"].is_array()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "splashes is malformed." << std::endl;
            splashes["splahses"] = {"Yuridrop On Top!"};    
        }

        return splashes;

    }();

    return splashes_config;

}

void loadCLI(std::string operating_system , dpp::snowflake guild_id) {

    /*
    Load the Command Line Interface.
    */

    const auto &banner_splashes = get_splashes_config()["splashes"];

    if (!banner_splashes.is_object() || banner_splashes.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "There are no valid splashes." << std::endl;
        return;
    }

    std::vector<std::string> keys;
    for (auto it = banner_splashes.begin(); it != banner_splashes.end(); ++it) {
        keys.push_back(it.key());
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0 , keys.size() - 1);
    std::string chosen_key = keys[dist(gen)];

    const auto &chosen_splash = banner_splashes[chosen_key];

    if (!chosen_splash.contains("quote") || !chosen_splash["quote"].is_string()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "The splash quote is not valid." << std::endl;
        return;
    }

    if (!chosen_splash.contains("author") || !chosen_splash["author"].is_string()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "The splash author is not valid." << std::endl;
        return;
    }

    std::string quote = chosen_splash["quote"];
    std::string author = chosen_splash["author"];

    if (operating_system == "windows") {
        std::system("cls");
    }
    else if (operating_system == "linux") {
        std::system("clear");
    }

    std::string bannerASCII = R"(
 ███▄    █  ▒█████   ███▄    █  ▄▄▄      
 ██ ▀█   █ ▒██▒  ██▒ ██ ▀█   █ ▒████▄    
▓██  ▀█ ██▒▒██░  ██▒▓██  ▀█ ██▒▒██  ▀█▄  
▓██▒  ▐▌██▒▒██   ██░▓██▒  ▐▌██▒░██▄▄▄▄██ 
▒██░   ▓██░░ ████▓▒░▒██░   ▓██░ ▓█   ▓██▒
░ ▒░   ▒ ▒ ░ ▒░▒░▒░ ░ ▒░   ▒ ▒  ▒▒   ▓▒█░
░ ░░   ░ ▒░  ░ ▒ ▒░ ░ ░░   ░ ▒░  ▒   ▒▒ ░
   ░   ░ ░ ░ ░ ░ ▒     ░   ░ ░   ░   ▒   
         ░     ░ ░           ░       ░  ░
)";

    std::cout << MAG << bannerASCII << std::endl;
    std::cout << MAG << quote << " - " << author << "\n" << std::endl;
    while (true) {
        if(!precache_done) {
            continue;
        }
        else {
            break;
        }
    }

    std::cout << "[ 🖥️ ] Server ID     : " << guild_id << std::endl;
    std::cout << "[ 💻 ] Server Name   : " << guild_name << std::endl;
    std::cout << "[ 🧍 ] Member Count  : " << members.size() << std::endl;
    std::cout << "[ 🏅 ] Role Count    : " << roles.size() << std::endl;
    std::cout << "[ 🏠 ] Channel Count : " << channels.size() << std::endl;
    std::cout << "[ 😎 ] Emoji Count   : " << emojis.size() << std::endl;
    std::cout << "[ 🖼️ ] Sticker Count : " << stickers.size() << std::endl;

    while (true) {

        #ifdef _WIN64
            #include <windows.h>
            sleep(1);
        #elif __linux__
            #include <unistd.h>
            sleep(1);
        #endif
        
        std::cout << MAG << "\n[ " << get_current_time() << " ]" << WHT << " Please enter a command: ";

        std::string line;
        std::getline(std::cin , line);

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        command = lowerString(command);

        std::vector<std::string> args;
        std::string word;
        while (iss >> word) {
            args.push_back(word);
        }

        // Command maps

        std::unordered_map<std::string, std::function<void(const std::string&)>> basic_functions = {
            {"help", [](const std::string &arg) { help(arg); }}
        };

        std::unordered_map<std::string, std::function<void(dpp::snowflake , std::optional<dpp::snowflake> , std::optional<int>)>> discord_functions = {
            {"createchannel" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_create_channel_job(guild_id);
            }},
            {"createchannels" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_create_channels_job(guild_id);
            }},
            {"deletechannel" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> channel_to_delete_id , std::optional<int>) {
                if (channel_to_delete_id) {
                    push_delete_channel_job(guild_id , *channel_to_delete_id);
                    auto it = std::find(channels.begin() , channels.end() , channel_to_delete_id); it != channels.end();
                    int index = std::distance(channels.begin() , it);
                    channels.erase(channels.begin() + index);
                    channel_names.erase(channel_names.begin() + index);
                }
                else {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] Missing the channel_to_delete_id parameter for deletechannel." << std::endl;
                }
            }},
            {"deletechannels" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_delete_channels_job(guild_id);
            }},
            {"spamchannel" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> channel_id , std::optional<int> spam_count) {
                if (channel_id) {
                    if (spam_count) {
                        push_spam_channel_job(guild_id , *channel_id , *spam_count);
                    }
                    else {
                        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] Missing the spam_count parameter for spamchannel." << std::endl;
                    }
                }
                else {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] Missing the channel_id parameter for spamchannel." << std::endl;
                }
            }},
            {"spamchannels" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_spam_channels_job(guild_id);
            }},
            {"banmember" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> user_id , std::optional<int>) {
                if (user_id) {
                    push_ban_member_job(guild_id , *user_id);
                    auto it = std::find(members.begin() , members.end() , user_id);
                    if (it != members.end()) {
                        int index = std::distance(members.begin() , it);
                        members.erase(members.begin() + index);
                        member_names.erase(member_names.begin() + index);
                    }
                }
                else {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] Missing the user_id parameter for banmember." << std::endl;
                }
            }},
            {"banmembers" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_ban_members_job(guild_id);
            }},
            {"kickmember" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> user_id , std::optional<int>) {
                if (user_id) {
                    push_kick_member_job(guild_id , *user_id);
                    auto it = std::find(members.begin() , members.end() , user_id); it != members.end();
                    int index = std::distance(members.begin() , it);
                    members.erase(members.begin() + index);
                    member_names.erase(member_names.begin() + index);
                }
                else {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] Missing the user_id parameter for kickmember." << std::endl;
                }
            }},
            {"kickmembers" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_kick_members_job(guild_id);
            }},
            {"givepermissions" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_give_permissions_job(guild_id);
            }},
            {"createrole" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_create_role_job(guild_id);
            }},
            {"createroles" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_create_roles_job(guild_id);
            }},
            {"deleterole" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> role_to_delete_id , std::optional<int>) {
                if (role_to_delete_id) {
                    push_delete_role_job(guild_id , *role_to_delete_id);
                    auto it = std::find(roles.begin() , roles.end() , *role_to_delete_id);
                    if (it != roles.end()) {
                        int index = std::distance(roles.begin() , it);
                        roles.erase(roles.begin() + index);
                        role_names.erase(role_names.begin() + index);
                    }
                }
                else {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] Missing the role_to_delete_id parameter for deleterole." << std::endl;
                }
            }},
            {"deleteroles" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_delete_roles_job(guild_id);
            }},
            {"createemoji" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_create_emoji_job(guild_id);
            }},
            {"createemojis" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_create_emojis_job(guild_id);
            }},
            {"deleteemoji" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> emoji_id , std::optional<int>) {
                if (emoji_id) {
                    push_delete_emoji_job(guild_id , *emoji_id);
                    auto it = std::find(emojis.begin() , emojis.end() , *emoji_id);
                    if (it != emojis.end()) {
                        int index = std::distance(emojis.begin() , it);
                        emojis.erase(roles.begin() + index);
                        emoji_names.erase(emoji_names.begin() + index);
                    }
                }
                else {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] Missing the role_to_delete_id parameter for deleterole." << std::endl;
                }
            }},
            {"deleteemojis" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_delete_emojis_job(guild_id);
            }},
            {"createsticker" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_create_sticker_job(guild_id);
            }},
            {"createstickers" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_create_stickers_job(guild_id);
            }},
            {"nuke" , [](dpp::snowflake guild_id , std::optional<dpp::snowflake> , std::optional<int>) {
                push_nuke_job(guild_id);
            }},

        };

        if (!validate(command)) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] " << command << " is not a valid command. To see a list of commands please use \"help\"." << std::endl;
            continue;
        }

        std::optional<dpp::snowflake> optional_parameter;
        std::optional<int> optional_parameter_2;
        if (!args.empty()) {
            optional_parameter = dpp::snowflake(args[0]);
        }
        if (args.size() > 1) {
            try {
                optional_parameter_2 = std::stoi(args[1]);
            }
            catch (const std::exception error) {
                std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] Could not convert the second argument to type integer: " << error.what() << std::endl;
            }
        }

        try {
            auto discord_fn = discord_functions.find(command);
            auto basic_fn = basic_functions.find(command);

            if (discord_fn != discord_functions.end()) {
                discord_fn -> second(guild_id , optional_parameter , optional_parameter_2);
            }
            else if (basic_fn != basic_functions.end()) {
                std::string arg = args.empty() ? "default" : args[0];
                basic_fn -> second(arg);
            }
            else {
                std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] No handler found for command: " << command << std::endl;
            }

        } 
        catch (const std::exception& error) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ loadCLI.cpp ] Unable to run the command " << command << ": " << error.what() << std::endl;
        }
    }
}