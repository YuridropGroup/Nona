// Standard library imports.

#include <iostream>
#include <string>
#include <chrono>

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../interface/command_worker.h"
#include "../../misc/colours.h"
#include "../../misc/time.h"
#include "../../misc/read_file.h"
#include "../../redis/precaching.h"
#include "../../redis/rate_limiter.h"

int created_emojis;
std::mutex created_emoji_mutex;

void create_emoji(dpp::cluster &Nona , const dpp::snowflake guild_id , const std::string emoji_name , std::shared_ptr<RateLimiter> limiter) {

    /*
    Create an emoji in the server.
    */

    limiter -> acquire();

    std::string selected_image = return_random_emoji_file();
    if (selected_image.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::create.cpp ] There are no images in the emojis folder." << std::endl;
        return;
    }

    std::vector<uint8_t> data = read_file(selected_image);

    if (data.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::create.cpp ] Unable to read the image data." << std::endl;
        return;        
    }

    dpp::emoji new_emoji;
    new_emoji.name = emoji_name;

    // Find image extension.

    dpp::image_type type = dpp::i_png;
    if (selected_image.ends_with(".jpg") || selected_image.ends_with(".jpeg")) {
        type = dpp::i_jpg;
    }
    else if (selected_image.ends_with(".gif")) {
        // Support GIF's at a later update.
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::create.cpp ] Nona does not currently support GIF's, please remove the GIF from the folder." << std::endl;
        return;
    }
    else {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::create.cpp ] An invalid file format was found in the emojis folder." << std::endl;
        return;
    }

    // Load the image into the emoji.

    std::string image_blob(reinterpret_cast<const char*>(data.data()), data.size());

    new_emoji.load_image(image_blob , type);

    if (new_emoji.image_data.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::create.cpp ] The emoji's image data is empty." << std::endl;
        return;        
    }
    if (new_emoji.image_data.size > (256 * 1024)) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::create.cpp ] The emoji's image size is is too large (> 256kb)." << std::endl;
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    try {
        Nona.guild_emoji_create(guild_id , new_emoji , [emoji_name , start](const dpp::confirmation_callback_t &callback) {
            auto end = std::chrono::high_resolution_clock::now();
            if (callback.is_error()) {
                std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::create.cpp ] An error occurred creating the emoji " << emoji_name << ": " << callback.get_error().message << std::endl;
                ++created_emojis;
            }
            else {
                std::chrono::duration<double> elapsed = end - start;
                std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Created the emoji " << emoji_name << " in " << elapsed.count() << " seconds." << std::endl;
                const dpp::emoji &created_emoji = std::get<dpp::emoji>(callback.value);
                std::lock_guard<std::mutex> lock(created_emoji_mutex);
                emojis.push_back(created_emoji.id);
                emoji_names.push_back(created_emoji.name);
                ++created_emojis;
            }
        });
    }
    catch (std::exception error) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ emojis::create.cpp ] Unable to create the emoji " << emoji_name << ": " << error.what() << std::endl;
    }
}