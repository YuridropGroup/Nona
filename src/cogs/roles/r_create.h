#ifndef R_CREATE_H
#define R_CREATE_H

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../redis/rate_limiter.h"
extern std::mutex created_role_mutex;
extern int created_roles;

void create_role(dpp::cluster &Nona , const dpp::snowflake guild_id , const std::string role_name , std::shared_ptr<RateLimiter> limiter);

#endif