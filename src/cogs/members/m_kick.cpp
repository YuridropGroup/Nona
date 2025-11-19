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

int kicked_members;

void kick_member(dpp::cluster &Nona , const dpp::snowflake guild_id , const dpp::snowflake user_id , std::shared_ptr<RateLimiter> limiter) {

    /*
    Kick a member from the server.
    */

    limiter -> acquire();

    auto it = std::find(members.begin() , members.end() , user_id);
    if (it != members.end()) {
        int index = std::distance(members.begin() , it);
        auto start = std::chrono::high_resolution_clock::now();
        try {
            Nona.guild_member_kick(guild_id , user_id , [user_id , index , start](const dpp::confirmation_callback_t &callback) {
                auto end = std::chrono::high_resolution_clock::now();
                const std::string kicked_member_name = member_names[index];
                if (callback.is_error()) {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ members::kick.cpp ] An error occurred kicking the member " << kicked_member_name << user_id << ": " << callback.get_error().message << std::endl;
                    ++kicked_members;
                }
                else {
                    std::chrono::duration<double> elapsed = end - start;
                    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Kicked the member " << kicked_member_name << " in " << elapsed.count() << " seconds." << std::endl;
                    ++kicked_members;
                }
            });
        }
        catch (std::exception error) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ members::kick.cpp ] Unable to kick the member with ID " << user_id << ": " << error.what() << std::endl;
        }
    }
    else {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ members::kick.cpp ] Unable to find the specified member ID to kick." << std::endl;        
    }
}