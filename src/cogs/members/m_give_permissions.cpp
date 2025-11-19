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

void give_permissions(dpp::cluster &Nona , const dpp::snowflake guild_id , std::shared_ptr<RateLimiter> limiter) {

    /*
    Give @everyone administrator in the server.
    */

    limiter -> acquire();

    try {
        dpp::role everyone_role;
        everyone_role.id = guild_id;
        everyone_role.guild_id = guild_id;
        everyone_role.permissions = dpp::p_administrator;
        auto start = std::chrono::high_resolution_clock::now();
        Nona.role_edit(everyone_role , [start](const dpp::confirmation_callback_t &callback) {
            auto end = std::chrono::high_resolution_clock::now();
            if (callback.is_error()) {
                std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ members::give_permissions.cpp ] An error occurred changing the permissions for @everyone: " << callback.get_error().message << std::endl;
            }
            else {
                std::chrono::duration<double> elapsed = end - start;
                std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Changed the permissions for @everyone" << " in " << elapsed.count() << " seconds." << std::endl;
            }
        });
    }
    catch (std::exception error) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ members::give_permissions.cpp ] Unable to edit the @everyone role: " << error.what() << std::endl;
    }

}