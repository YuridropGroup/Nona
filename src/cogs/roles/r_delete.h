#ifndef R_DELETE_H
#define R_DELETE_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"

extern int deleted_roles;

void delete_role(dpp::cluster &Nona , dpp::snowflake guild_id , dpp::snowflake role_to_delete_id , std::shared_ptr<RateLimiter> limiter);

#endif