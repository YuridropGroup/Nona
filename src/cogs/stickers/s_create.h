#ifndef S_CREATE_H
#define S_CREATE_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"

void create_sticker(dpp::cluster &Nona , const dpp::snowflake guild_id , const std::string sticker_name , std::shared_ptr<RateLimiter> limiter);

#endif