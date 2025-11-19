#ifndef CLIENT_H
#define CLIENT_H

#include "dpp/dpp.h"

#include "redis/rate_limiter.h"

void loadEvents(dpp::cluster &Nona , int clientNum , int total_bots , std::shared_ptr<RateLimiter> limiter);
void loadCogs(dpp::cluster &Nona);
void loadCommand(dpp::cluster &Nona , int command , dpp::snowflake guild_id);

#endif