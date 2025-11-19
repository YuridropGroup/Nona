#ifndef E_DELETE_H
#define E_DELETE_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"

extern int deleted_emojis;

void delete_emoji(dpp::cluster &Nona , dpp::snowflake guild_id , dpp::snowflake channel_to_delete_id , std::shared_ptr<RateLimiter> limiter);

#endif