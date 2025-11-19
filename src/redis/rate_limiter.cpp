// Standard library imports.

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

// External library imports.

#include <nlohmann/json.hpp>
#include <dpp/dpp.h>

// Custom module imports.

#include "../misc/colours.h"
#include "../misc/time.h"
#include "../cogs/channels/c_create.h"

RateLimiter::RateLimiter(int max_RPS) : max_requests_per_second(max_RPS) , requests_this_second(0) , window_start(std::chrono::steady_clock::now()) {

}

void RateLimiter::acquire() {

    /*
    Acquire the rate-limit status of the client.
    */

    while (true) {

        auto now = std::chrono::steady_clock::now();
        if (now - window_start >= std::chrono::seconds(1)) {
            window_start = now;
            requests_this_second = 0;
        }
        if (requests_this_second < max_requests_per_second) {
            ++requests_this_second;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    }
}