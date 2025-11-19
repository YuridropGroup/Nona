#ifndef COMMAND_WORKER_H
#define COMMAND_WORKER_H

#include <string>
#include <dpp/dpp.h>

std::string return_channel_name();
dpp::snowflake return_random_random_channel();
std::string return_random_spam_message();
std::string return_random_role_name();
std::string return_random_emoji_file();
std::string return_random_sticker_file();
std::string return_random_emoji_name();
std::string return_random_sticker_name();

void push_create_channel_job(const dpp::snowflake &guild_id);
void push_create_channels_job(const dpp::snowflake &guild_id);
void push_delete_channel_job(const dpp::snowflake &guild_id , dpp::snowflake &channel_to_delete_id);
void push_delete_channels_job(const dpp::snowflake &guild_id);
void push_spam_channel_job(const dpp::snowflake &guild_id , dpp::snowflake &channel_to_spam_id , int spam_count);
void push_spam_channels_job(const dpp::snowflake &guild_id);
void push_ban_member_job(const dpp::snowflake &guild_id , dpp::snowflake &user_id);
void push_ban_members_job(const dpp::snowflake &guild_id);
void push_kick_member_job(const dpp::snowflake &guild_id , dpp::snowflake &user_id);
void push_kick_members_job(const dpp::snowflake &guild_id);
void push_give_permissions_job(const dpp::snowflake &guild_id);
void push_create_role_job(const dpp::snowflake &guild_id);
void push_create_roles_job(const dpp::snowflake &guild_id);
void push_delete_role_job(const dpp::snowflake &guild_id , dpp::snowflake &role_to_delete_id);
void push_delete_roles_job(const dpp::snowflake &guild_id);
void push_create_emoji_job(const dpp::snowflake &guild_id);
void push_create_emojis_job(const dpp::snowflake &guild_id);
void push_delete_emoji_job(const dpp::snowflake &guild_id , dpp::snowflake &emoji_id);
void push_delete_emojis_job(const dpp::snowflake &guild_id);
void push_create_sticker_job(const dpp::snowflake &guild_id);
void push_create_stickers_job(const dpp::snowflake &guild_id);
void push_nuke_job(const dpp::snowflake &guild_id);

#endif