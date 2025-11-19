// Standard library imports.

#include <iostream>
#include <string>

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "client.h"
#include "events/on_ready.h"

void loadEvents(dpp::cluster &Nona , int clientNum , int total_bots , std::shared_ptr<RateLimiter> limiter) {

    /*
    Load all of Nona's events.
    */

    on_ready_event(Nona , clientNum , total_bots , limiter);

}

void loadCogs(dpp::cluster &Nona) {

    /*
    Load all of Nona's cogs.
    */
   
}

void loadCommand(dpp::cluster &Nona , int command , dpp::snowflake guild_id) {

    /*
    Run the command that is chosen.
    */

}