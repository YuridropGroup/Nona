// rate_limiter.h
#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <atomic>
#include <chrono>

class RateLimiter {

    public:

        RateLimiter(int max_RPS);
        void acquire();

    private:

        int max_requests_per_second;
        std::atomic<int> requests_this_second;
        std::chrono::steady_clock::time_point window_start;

};

#endif