// Standard library imports.

#include <iostream>
#include <string>
#include <chrono>

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../misc/colours.h"
#include "../../misc/time.h"
#include "../../redis/precaching.h"
#include "../../redis/rate_limiter.h"

void spam_channel(dpp::cluster &Nona , dpp::snowflake guild_id , dpp::snowflake channel_id , std::string message , std::shared_ptr<RateLimiter> limiter) {

    /*
    Spam a channel in the server.
    */

    limiter -> acquire();

    if (std::find(channels.begin() , channels.end() , channel_id) != channels.end()) {
        try {
            dpp::message spam_message(channel_id , message);
            spam_message.set_allowed_mentions(false , false , true);
            Nona.message_create(spam_message , [channel_id](const dpp::confirmation_callback_t &callback) {
                auto end = std::chrono::high_resolution_clock::now();
                if (callback.is_error()) {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ channels::c_spam.cpp ] An error occurred spamming the channel with ID " << channel_id << ": " << callback.get_error().message << std::endl;
                }
            });
        }
        catch (std::exception error) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ channels::c_spam.cpp ] Unable to spam the channel with ID " << channel_id << ": " << error.what() << std::endl;
        }
    }
    else {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ channels::c_spam.cpp ] Unable to find the specified channel ID to spam." << std::endl;        
    }

}