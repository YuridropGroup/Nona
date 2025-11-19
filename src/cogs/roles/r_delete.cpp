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

int deleted_roles;

void delete_role(dpp::cluster &Nona , const dpp::snowflake guild_id , const dpp::snowflake role_to_delete_id , std::shared_ptr<RateLimiter> limiter) {

    /*
    Delete a role in the server.
    */

    limiter -> acquire();

    auto it = std::find(roles.begin() , roles.end() , role_to_delete_id);
    if (it != roles.end()) {
        int index = std::distance(roles.begin() , it);
        auto start = std::chrono::high_resolution_clock::now();
        try {
            Nona.role_delete(guild_id , role_to_delete_id , [role_to_delete_id , index , start](const dpp::confirmation_callback_t &callback) {
                auto end = std::chrono::high_resolution_clock::now();
                const std::string deleted_role_name = role_names[index];
                if (callback.is_error()) {
                    std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ roles::delete.cpp ] An error occurred deleting the role " << deleted_role_name << ": " << callback.get_error().message << std::endl;
                    ++deleted_roles;
                }
                else {
                    std::chrono::duration<double> elapsed = end - start;
                    std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Deleted the role " << deleted_role_name << " in " << elapsed.count() << " seconds." << std::endl;
                    ++deleted_roles;
                }
            });
        }   
        catch (std::exception error) {
            std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ roles::delete.cpp ] Unable to delete the role with ID " << role_to_delete_id << ": " << error.what() << std::endl;
        }
    }
    else {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ roles::delete.cpp ] Unable to find the specified role ID to delete." << std::endl;        
    }

}