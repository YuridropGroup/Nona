#ifndef PRECACHE_H
#define PRECACHE_H

// Standard library imports.

#include <string>
#include <atomic>

// External library imports.

#include <dpp/dpp.h>

// Thread locking variables.

extern std::atomic<bool> precache_done;
extern std::atomic<int> callbacks_done;
extern const int total_callbacks;

// Precaching variables.

extern std::string guild_name;
extern std::vector<dpp::snowflake> channels;
extern std::vector<std::string> channel_names;
extern std::vector<dpp::snowflake> roles;
extern std::vector<std::string> role_names;
extern std::vector<dpp::snowflake> members;
extern std::vector<std::string> member_names;
extern std::vector<dpp::snowflake> emojis;
extern std::vector<std::string> emoji_names;
extern std::vector<dpp::snowflake> stickers;
extern std::vector<std::string> sticker_names;

extern dpp::snowflake last_user_id;

void fetch_members_page(dpp::cluster &Nona , dpp::snowflake guild_id);
void precache_objects(dpp::cluster &Nona , dpp::snowflake guild_id_str);

#endif