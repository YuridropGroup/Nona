// Standard library imports.

#include <iostream>
#include <string>
#include <chrono>
#include <mutex>

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../misc/colours.h"
#include "../../misc/time.h"
#include "../../redis/precaching.h"
#include "../../redis/rate_limiter.h"

std::mutex created_channels_mutex;
int created_channels;

void create_channel(dpp::cluster &Nona , const dpp::snowflake guild_id , const std::string channel_name , const std::string channel_topic , std::shared_ptr<RateLimiter> limiter) {

    /*
    Create a channel in the server.
    */

    limiter -> acquire();

    dpp::channel new_channel;
    new_channel.guild_id = guild_id;
    new_channel.name = channel_name;
    new_channel.topic = channel_topic;
    new_channel.set_type(dpp::channel_type::CHANNEL_TEXT);

    auto start = std::chrono::high_resolution_clock::now();
    try {
        Nona.channel_create(new_channel , [channel_name , start](const dpp::confirmation_callback_t &callback) {
            auto end = std::chrono::high_resolution_clock::now();
            if (callback.is_error()) {
                std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ channels::c_create.cpp ] An error occurred creating the channel " << channel_name << ": " << callback.get_error().message << std::endl;
                ++created_channels;
            }
            else {
                std::chrono::duration<double> elapsed = end - start;
                std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Created the channel " << channel_name << " in " << elapsed.count() << " seconds." << std::endl;
                const dpp::channel &created_channel = std::get<dpp::channel>(callback.value);
                std::lock_guard<std::mutex> lock(created_channels_mutex);
                channels.push_back(created_channel.id);
                channel_names.push_back(created_channel.name);
                ++created_channels;
            }
        });
    }
    catch (std::exception error) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ channels::c_create.cpp ] Unable to create the channel " << channel_name << ": " << error.what() << std::endl;
    }
}