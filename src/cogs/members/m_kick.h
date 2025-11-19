#ifndef M_KICK_H
#define M_KICK_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"

extern int kicked_members;

void kick_member(dpp::cluster &Nona , const dpp::snowflake guild_id , const dpp::snowflake user_id , std::shared_ptr<RateLimiter> limiter);

#endif