#ifndef C_CREATE_H
#define C_CREATE_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"

extern std::mutex created_channels_mutex;
extern int created_channels;

void create_channel(dpp::cluster &Nona , const dpp::snowflake guild_id , const std::string channel_name , const std::string channel_topic , std::shared_ptr<RateLimiter> limiter);

#endif