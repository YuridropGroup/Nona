// Standard library imports.

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

// External library imports.

#include "dpp/dpp.h"
#include "sw/redis++/redis++.h"

// Custom module imports.

#include "../misc/colours.h"
#include "../misc/time.h"
#include "../redis/dispatcher.h"
#include "../redis/precaching.h"
#include "../redis/rate_limiter.h"

std::mutex ready_mutex;
std::condition_variable all_ready_cv;
int ready_bots = 0;
std::vector<dpp::snowflake> client_ids;

void on_ready_event(dpp::cluster& Nona , int client_number , int total_bots , std::shared_ptr<RateLimiter> limiter) {

    /*
    The event that is called for each iteration of Nona.
    */

    Nona.on_ready([&Nona , client_number , total_bots , limiter](const dpp::ready_t& event) {

        try {
            dpp::activity botActivity;
            botActivity.type = dpp::activity_type::at_game;
            botActivity.name = "EZ4Nona...";

            Nona.set_presence(dpp::presence(dpp::ps_online , botActivity));
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
            std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Nona #" << client_number << " (" << Nona.me.username << ") is online." << std::endl;
            client_ids.push_back(Nona.me.id);

            // Redis logic

            std::thread worker([&Nona , limiter]() {
                try {
                    sw::redis::Redis redis("tcp://127.0.0.1:6379/0");
                    while (true) {
                        auto reply = redis.blpop("job_queue", 0);
                        if (reply) {
                            std::string job_string = reply -> second;
                            dispatch_job(Nona , job_string , limiter);
                        }
                    }
                } catch (const std::exception &error) {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ on_ready.cpp ] Redis error: " << error.what() << std::endl;
                }
            });
            worker.detach();

            {
                std::lock_guard<std::mutex> lock(ready_mutex);
                ready_bots++;
                if (ready_bots == total_bots) {
                    all_ready_cv.notify_one();
                }
            }
        } catch (const std::exception& error) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ on_ready.cpp ] Error in on_ready for Nona #" << client_number << ": " << error.what() << std::endl;
        }
    });
}