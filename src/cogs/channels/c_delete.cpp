// Standard library imports.

#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <vector>
#include <mutex>

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../misc/colours.h"
#include "../../misc/time.h"
#include "../../redis/precaching.h"
#include "../../redis/rate_limiter.h"

int deleted_channels;

void delete_channel(dpp::cluster &Nona , const dpp::snowflake guild_id , const dpp::snowflake channel_id , std::shared_ptr<RateLimiter> limiter) {

    /*
    Delete a channel in the server.
    */

    limiter -> acquire();

    auto it = std::find(channels.begin() , channels.end() , channel_id);
    if (it != channels.end()) {
        int index = std::distance(channels.begin() , it);
        auto start = std::chrono::high_resolution_clock::now();
        try {
            Nona.channel_delete(channel_id , [channel_id , index , start](const dpp::confirmation_callback_t &callback) {
                auto end = std::chrono::high_resolution_clock::now();
                const std::string deleted_channel_name = channel_names[index];
                if (callback.is_error()) {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ channels::c_delete.cpp ] An error occurred deleting the channel " << deleted_channel_name << ": " << callback.get_error().message << std::endl;
                    ++deleted_channels;
                }
                else {
                    std::chrono::duration<double> elapsed = end - start;
                    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Deleted the channel " << deleted_channel_name << " in " << elapsed.count() << " seconds." << std::endl;
                    ++deleted_channels;
                }
            });
        }   
        catch (std::exception error) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ channels::c_delete.cpp ] Unable to delete the channel with ID " << channel_id << ": " << error.what() << std::endl;
        }
    }
    else {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ channels::c_delete.cpp ] Unable to find the specified channel ID to delete." << std::endl;        
    }

}