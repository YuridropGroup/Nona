// Standard library imports.

#include <iostream>
#include <string>
#include <chrono>
#include <string_view>

// External library imports.

#include <dpp/dpp.h>

// Custom module imports.

#include "../../interface/command_worker.h"
#include "../../misc/colours.h"
#include "../../misc/time.h"
#include "../../misc/read_file.h"
#include "../../redis/precaching.h"
#include "../../redis/rate_limiter.h"

void create_sticker(dpp::cluster &Nona , const dpp::snowflake guild_id , const std::string sticker_name , std::shared_ptr<RateLimiter> limiter) {

    /*
    Create a sticker in the server.
    */

    limiter -> acquire();

    std::string selected_image = return_random_sticker_file();
    if (selected_image.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ stickers::create.cpp ] There are no images in the stickers folder." << std::endl;
        return;
    }

    std::vector<uint8_t> data = read_file(selected_image);

    if (data.empty()) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ stickers::create.cpp ] Unable to read the image data." << std::endl;
        return;        
    }

    dpp::sticker new_sticker;
    new_sticker.guild_id = guild_id;
    new_sticker.name = sticker_name;
    new_sticker.description = "PWNd by Yuridrop!";
    new_sticker.filename = selected_image;
    new_sticker.tags = "radioactive";

    // Find image extension.
    
    if (selected_image.ends_with(".png")) {
        new_sticker.format_type = dpp::sticker_format::sf_png;
    }
    else if (selected_image.ends_with(".jpg") || selected_image.ends_with(".jpeg")) {
        new_sticker.format_type = dpp::sticker_format::sf_png;
    }
    else if (selected_image.ends_with(".gif")) {
        // Support GIF's at a later update.
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ stickers::create.cpp ] Nona does not currently support GIF's, please remove the GIF from the folder." << std::endl;
        return;
    }
    else {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ stickers::create.cpp ] An invalid file format was found in the stickers folder." << std::endl;
        return;
    }

    if (data.size() > (512 * 1024)) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ stickers::create.cpp ] The sticker's image size is is too large (> 512)." << std::endl;
        return;
    }

    // Load the image into the sticker.

    std::string file_content(data.begin(), data.end());
    new_sticker.set_file_content(file_content);

    auto start = std::chrono::high_resolution_clock::now();
    try {
        Nona.guild_sticker_create(new_sticker , [sticker_name , start](const dpp::confirmation_callback_t &callback) {
            auto end = std::chrono::high_resolution_clock::now();
            if (callback.is_error()) {
                std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ stickers::create.cpp ] An error occurred creating the sticker " << sticker_name << ": " << callback.get_error().message << std::endl;
            }
            else {
                std::chrono::duration<double> elapsed = end - start;
                std::cout << MAG << "[ " << get_current_time() << " ] " << GRN << "Created the sticker " << sticker_name << " in " << elapsed.count() << " seconds." << std::endl;
                const dpp::sticker &created_sticker = std::get<dpp::sticker>(callback.value);
                stickers.push_back(created_sticker.id);
            }
        });
    }
    catch (std::exception error) {
        std::cout << MAG << "[ " << get_current_time() << " ] " << RED << "[ ERROR ][ stickers::create.cpp ] Unable to create the sticker " << sticker_name << ": " << error.what() << std::endl;
    }
}