#ifndef E_CREATE_H
#define E_CREATE_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"

extern int created_emojis;
extern std::mutex created_emoji_mutex;

void create_emoji(dpp::cluster &Nona , const dpp::snowflake guild_id , const std::string channel_name , std::shared_ptr<RateLimiter> limiter);

#endif