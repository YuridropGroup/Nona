// Standard library imports.

#include <iostream>
#include <vector>
#include <string>
#include <atomic>
#include <unordered_map>

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../events/on_ready.h"
#include "../misc/colours.h"
#include "../misc/time.h"

std::atomic<bool> precache_done(false);
std::atomic<int> callbacks_done(0);
const int total_callbacks = 6;

std::string guild_name;
std::vector<dpp::snowflake> channels;
std::vector<std::string> channel_names;
std::vector<dpp::snowflake> roles;
std::vector<std::string> role_names;
std::vector<dpp::snowflake> members;
std::vector<std::string> member_names;
std::vector<dpp::snowflake> emojis;
std::vector<std::string> emoji_names;
std::vector<dpp::snowflake> stickers;
std::vector<std::string> sticker_names;

dpp::snowflake last_user_id;

void fetch_members_page(dpp::cluster &Nona , dpp::snowflake guild_id) {

    /*
    Call recursively to fetch each pag
    e of members (only used for guilds with over 1000 members).
    */

    Nona.guild_get_members(guild_id , 1000 , last_user_id , [&Nona , &guild_id](const dpp::confirmation_callback_t &callback) {
        if (callback.is_error()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "An error occurred caching members: " << callback.get_error().message << std::endl;
            return;
        }       
        else {
            auto &mapped_members = std::get<dpp::guild_member_map>(callback.value);

            if (mapped_members.empty()) {
                return;
            }
            for (auto &[id , member] : mapped_members) {
                last_user_id = std::max(last_user_id , id);
                if (!(std::find(client_ids.begin() , client_ids.end() , id) != client_ids.end())) {
                    members.push_back(id);
                    if (member.get_user()) {
                        member_names.push_back(member.get_user() -> username);
                    }
                    else {
                        member_names.push_back("Unidentified Member");
                    }
                }
            }
            if (mapped_members.size() == 1000) {
                fetch_members_page(Nona , guild_id);
            }
            else {
                if (++callbacks_done == total_callbacks) precache_done = true;
            }
        } 
    });

}

void precache_objects(dpp::cluster &Nona , dpp::snowflake guild_id) {

    /*
    Precache all Discord objects (channels etc).
    */

    // chs = channels
    // rle = roles
    // mbs = members
    // ejs = emojis
    // sks = stickers

    // Get server name.

    Nona.guild_get(guild_id , [](const dpp::confirmation_callback_t &callback) {
        if (callback.is_error()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "An error occurred caching channels: " << callback.get_error().message << std::endl;
        }
        else {
            guild_name.clear();
            const dpp::guild &guild = std::get<dpp::guild>(callback.value);
            guild_name = guild.name;
        }
        if (++callbacks_done == total_callbacks) precache_done = true;
    });

    // Get all channels.

    Nona.channels_get(guild_id , [&Nona](const dpp::confirmation_callback_t &callback) {
        if (callback.is_error()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "An error occurred caching channels: " << callback.get_error().message << std::endl;
        }
        else {
            channels.clear();
            channel_names.clear();
            std::unordered_map<dpp::snowflake , dpp::channel> chs = std::get<std::unordered_map<dpp::snowflake , dpp::channel>>(callback.value);
            for (const auto &[id , channel] : chs) {
                channels.push_back(id);
                channel_names.push_back(channel.name);
            }
        }
        if (++callbacks_done == total_callbacks) precache_done = true;
    });

    // Get all roles.

    Nona.roles_get(guild_id , [&Nona , guild_id](const dpp::confirmation_callback_t &callback) {
        if (callback.is_error()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "An error occurred caching roles: " << callback.get_error().message << std::endl;
        }
        else {
            roles.clear();
            role_names.clear();
            std::unordered_map<dpp::snowflake , dpp::role> rle = std::get<std::unordered_map<dpp::snowflake , dpp::role>>(callback.value);
            for (const auto &[id , role] : rle) {
                if (id != guild_id) {
                    roles.push_back(id);
                    role_names.push_back(role.name);
                }
            }
        }
        if (++callbacks_done == total_callbacks) precache_done = true;
    });

    // Get all members

    fetch_members_page(Nona , guild_id);

    // Get all emojis.

    Nona.guild_emojis_get(guild_id , [&Nona](const dpp::confirmation_callback_t &callback) {
        if (callback.is_error()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "An error occurred caching emojis: " << callback.get_error().message << std::endl;
        }
        else {
            emojis.clear();
            emoji_names.clear();
            std::unordered_map<dpp::snowflake , dpp::emoji> ejs = std::get<std::unordered_map<dpp::snowflake , dpp::emoji>>(callback.value);
            for (const auto &[id , emoji] : ejs) {
                emojis.push_back(id);
                emoji_names.push_back(emoji.name);
            }
        }
        if (++callbacks_done == total_callbacks) precache_done = true;
    });

    // Get all stickers.

    Nona.guild_stickers_get(guild_id , [&Nona](const dpp::confirmation_callback_t &callback) {
        if (callback.is_error()) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "An error occurred caching stickers: " << callback.get_error().message << std::endl;
        }
        else {
            stickers.clear();
            sticker_names.clear();
            std::unordered_map<dpp::snowflake , dpp::sticker> sks = std::get<std::unordered_map<dpp::snowflake , dpp::sticker>>(callback.value);
            for (const auto &[id , sticker] : sks) {
                stickers.push_back(id);
                sticker_names.push_back(sticker.name);
            }
        }
        if (++callbacks_done == total_callbacks) precache_done = true;
    });

}