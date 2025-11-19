#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string>
#include <dpp/dpp.h>
#include "rate_limiter.h"

void dispatch_job(dpp::cluster &Nona , const std::string &job_json , std::shared_ptr<RateLimiter> limiter);

#endif