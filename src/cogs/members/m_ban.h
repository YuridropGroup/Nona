#ifndef M_BAN_H
#define M_BAN_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"

extern int banned_members;

void ban_member(dpp::cluster &Nona , const dpp::snowflake guild_id , const dpp::snowflake user_id , std::shared_ptr<RateLimiter> limiter);

#endif