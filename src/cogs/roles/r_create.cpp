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

std::mutex created_role_mutex;
int created_roles;

void create_role(dpp::cluster &Nona , const dpp::snowflake guild_id , const std::string role_name , std::shared_ptr<RateLimiter> limiter) {

    /*
    Create a role in the server.
    */

    limiter -> acquire();

    dpp::role new_role;
    new_role.guild_id = guild_id;
    new_role.name = role_name;
    new_role.colour = 0x000001;

    auto start = std::chrono::high_resolution_clock::now();
    try {
        Nona.role_create(new_role , [role_name , start](const dpp::confirmation_callback_t &callback) {
            auto end = std::chrono::high_resolution_clock::now();
            if (callback.is_error()) {
                std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ roles::create.cpp ] An error occurred creating the role " << role_name << ": " << callback.get_error().message << std::endl;
                ++created_roles;
            }
            else {
                std::chrono::duration<double> elapsed = end - start;
                std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Created the role " << role_name << " in " << elapsed.count() << " seconds." << std::endl;
                const dpp::role &created_role = std::get<dpp::role>(callback.value);
                std::lock_guard<std::mutex> lock(created_role_mutex);
                roles.push_back(created_role.id);
                role_names.push_back(created_role.name);
                ++created_roles;
            }
        });
    }
    catch (std::exception error) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ roles::create.cpp ] Unable to create the role " << role_name << ": " << error.what() << std::endl;
    }
}