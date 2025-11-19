// Standard library imports.

#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <vector>

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../misc/colours.h"
#include "../../misc/time.h"
#include "../../redis/precaching.h"
#include "../../redis/rate_limiter.h"

int deleted_emojis;

void delete_emoji(dpp::cluster &Nona , const dpp::snowflake guild_id , const dpp::snowflake emoji_id , std::shared_ptr<RateLimiter> limiter) {

    /*
    Delete an emoji in the server.
    */

    limiter -> acquire();

    auto it = std::find(emojis.begin() , emojis.end() , emoji_id);
    if (it != emojis.end()) {
        int index = std::distance(emojis.begin() , it);
        auto start = std::chrono::high_resolution_clock::now();
        try {
            Nona.guild_emoji_delete(guild_id , emoji_id , [emoji_id , index , start](const dpp::confirmation_callback_t &callback) {
                auto end = std::chrono::high_resolution_clock::now();
                const std::string deleted_emoji_name = emoji_names[index];
                if (callback.is_error()) {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::delete.cpp ] An error occurred deleting the emoji " << deleted_emoji_name << ": " << callback.get_error().message << std::endl;
                    ++deleted_emojis;
                }
                else {
                    std::chrono::duration<double> elapsed = end - start;
                    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Deleted the emoji " << deleted_emoji_name << " in " << elapsed.count() << " seconds." << std::endl;
                    ++deleted_emojis;
                }
            });
        }   
        catch (std::exception error) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::delete.cpp ] Unable to delete the channel with ID " << emoji_id << ": " << error.what() << std::endl;
        }
    }
    else {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::delete.cpp ] Unable to find the specified channel ID to delete." << std::endl;        
    }

}