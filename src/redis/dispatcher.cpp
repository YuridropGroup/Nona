// Standard library imports.

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

// External library imports.

#include <nlohmann/json.hpp>
#include <dpp/dpp.h>    

// Custom module imports.

#include "../misc/colours.h"
#include "../misc/time.h"
#include "../cogs/channels/c_create.h"
#include "../cogs/channels/c_delete.h"
#include "../cogs/channels/c_spam.h"
#include "../cogs/emojis/e_create.h"
#include "../cogs/emojis/e_delete.h"
#include "../cogs/members/m_ban.h"
#include "../cogs/members/m_kick.h"
#include "../cogs/members/m_give_permissions.h"
#include "../cogs/roles/r_create.h"
#include "../cogs/roles/r_delete.h"
#include "../cogs/stickers/s_create.h"
#include "../redis/rate_limiter.h"

using json = nlohmann::json;

void dispatch_job(dpp::cluster &Nona , const std::string &job_json , std::shared_ptr<RateLimiter> limiter) {

    /*
    Dispatch a job from the Redis queue to the command requested.
    */

    try {
        auto job = json::parse(job_json);
        auto args = job["args"];
        std::string command = job["command"];

        // Check commands here.
        
        if (command == "create_channel") {
            const dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            const std::string channel_name = args["channel_name"];
            const std::string channel_topic = args["channel_topic"];
            create_channel(Nona , guild_id , channel_name , channel_topic , limiter);
        }
        else if (command == "delete_channel") {
            dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            dpp::snowflake channel_id = std::stoull(args["channel_id"].get<std::string>());
            delete_channel(Nona , guild_id , channel_id , limiter);
        }
        else if (command == "spam_channel") {
            dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            dpp::snowflake channel_id = std::stoull(args["channel_id"].get<std::string>());
            std::string message = args["message"];
            spam_channel(Nona , guild_id , channel_id , message , limiter);
        }
        else if (command == "ban_member") {
            dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            dpp::snowflake user_id = std::stoull(args["user_id"].get<std::string>());
            ban_member(Nona , guild_id , user_id , limiter);
        }
        else if (command == "kick_member") {
            dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            dpp::snowflake user_id = std::stoull(args["user_id"].get<std::string>());
            kick_member(Nona , guild_id , user_id , limiter);
        }
        else if (command == "give_permissions") {
            dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            give_permissions(Nona , guild_id , limiter);
        }
        else if (command == "create_role") {
            dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            std::string name = args["name"];
            create_role(Nona , guild_id , name , limiter);
        }
        else if (command == "delete_role") {
            dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            dpp::snowflake role_id = std::stoull(args["role_id"].get<std::string>());
            delete_role(Nona , guild_id , role_id , limiter);
        }
        else if (command == "create_emoji") {
            dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            std::string name = args["name"];
            create_emoji(Nona , guild_id , name , limiter);
        }
        else if (command == "delete_emoji") {
            dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            dpp::snowflake emoji_id = std::stoull(args["emoji_id"].get<std::string>());
            delete_emoji(Nona , guild_id , emoji_id , limiter);
        }
        else if (command == "create_sticker") {
            dpp::snowflake guild_id = std::stoull(args["guild_id"].get<std::string>());
            std::string name = args["name"];
            create_sticker(Nona , guild_id , name , limiter);
        }
        
    }
    catch (const std::exception &error) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "An error occurred within Redis: " << error.what() << std::endl;
    }
}