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

int banned_members;

void ban_member(dpp::cluster &Nona , const dpp::snowflake guild_id , const dpp::snowflake user_id , std::shared_ptr<RateLimiter> limiter) {

    /*
    Ban a member from the server.
    */

    limiter -> acquire();

    auto it = std::find(members.begin() , members.end() , user_id);
    if (it != members.end()) {
        int index = std::distance(members.begin() , it);
        auto start = std::chrono::high_resolution_clock::now();
        try {
            Nona.guild_ban_add(guild_id , user_id , 0 , [user_id , index , start](const dpp::confirmation_callback_t &callback) {
                auto end = std::chrono::high_resolution_clock::now();
                const std::string banned_member_name = member_names[index];
                if (callback.is_error()) {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ members::m_ban.cpp ] An error occurred banning the member " << banned_member_name << user_id << ": " << callback.get_error().message << std::endl;
                    ++banned_members;
                }
                else {
                    std::chrono::duration<double> elapsed = end - start;
                    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Banned the member " << banned_member_name << " in " << elapsed.count() << " seconds." << std::endl;
                    ++banned_members;
                }
            });
        }
        catch (std::exception error) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ members::m_ban.cpp ] Unable to ban the member with ID " << user_id << ": " << error.what() << std::endl;
        }
    }
    else {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ members::m_ban.cpp ] Unable to find the specified member ID to ban." << std::endl;        
    }
}