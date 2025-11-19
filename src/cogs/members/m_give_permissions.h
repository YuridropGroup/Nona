#ifndef M_GIVE_PERMISSIONS_H
#define M_GIVE_PERMISSIONS_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"

void give_permissions(dpp::cluster &Nona , const dpp::snowflake guild_id , std::shared_ptr<RateLimiter> limiter);

#endif