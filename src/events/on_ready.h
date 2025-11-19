#ifndef ON_READY_H
#define ON_READY_H

#include <mutex>
#include <condition_variable>

#include "dpp/dpp.h"

#include "../redis/rate_limiter.h"

extern std::mutex ready_mutex;
extern std::condition_variable all_ready_cv;
extern int ready_bots;
extern std::vector<dpp::snowflake> client_ids;

void on_ready_event(dpp::cluster &Nona , int clientNum , int total_bots , std::shared_ptr<RateLimiter> limiter);
dpp::snowflake return_guild_id();

#endif