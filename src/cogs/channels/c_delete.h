#ifndef C_DELETE_H
#define C_DELETE_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"

extern int deleted_channels;

void delete_channel(dpp::cluster &Nona , dpp::snowflake guild_id , dpp::snowflake channel_id , std::shared_ptr<RateLimiter> limiter);

#endif