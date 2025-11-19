#ifndef C_SPAM_H
#define C_SPAM_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"

void spam_channel(dpp::cluster &Nona , dpp::snowflake guild_id , dpp::snowflake channel_id , std::string message , std::shared_ptr<RateLimiter> limiter);

#endif