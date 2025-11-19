cmake_minimum_required(VERSION 3.16)
project(Nona)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Place the binary in the root directory.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Change the path to your path for vcpkg (if you use this code).

set(CMAKE_TOOLCHAIN_FILE "/home/frieren/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Find packages via vcpkg

find_package(redis++ CONFIG REQUIRED)
find_package(dpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

# Source files.

set(SOURCES

    src/client.cpp
    src/index.cpp
    src/redis/dispatcher.cpp
    src/redis/precaching.cpp
    src/redis/rate_limiter.cpp
    src/misc/validate_command.cpp
    src/misc/help.cpp
    src/misc/lower_string.cpp
    src/misc/time.cpp
    src/misc/read_file.cpp
    src/interface/command_worker.cpp
    src/interface/loadCLI.cpp
    src/interface/loadGUI.cpp
    src/events/on_ready.cpp
    src/cogs/channels/c_create.cpp
    src/cogs/channels/c_delete.cpp
    src/cogs/channels/c_spam.cpp
    src/cogs/emojis/e_create.cpp
    src/cogs/emojis/e_delete.cpp
    src/cogs/members/m_ban.cpp
    src/cogs/members/m_kick.cpp
    src/cogs/members/m_give_permissions.cpp
    src/cogs/roles/r_create.cpp
    src/cogs/roles/r_delete.cpp
    src/cogs/stickers/s_create.cpp

)

# Header files.

set(HEADERS

    src/index.h
    src/client.h
    src/redis/dispatcher.h
    src/redis/precaching.h
    src/redis/rate_limiter.h
    src/misc/colours.h
    src/misc/validate_command.h
    src/misc/help.h
    src/misc/lower_string.h
    src/misc/time.h
    src/misc/read_file.h
    src/interface/command_worker.h
    src/interface/loadCLI.h
    src/interface/loadGUI.h
    src/events/on_ready.h
    src/cogs/channels/c_create.h
    src/cogs/channels/c_delete.h
    src/cogs/channels/c_spam.h
    src/cogs/emojis/e_create.h
    src/cogs/emojis/e_delete.h
    src/cogs/members/m_ban.h
    src/cogs/members/m_kick.h
    src/cogs/members/m_give_permissions.h
    src/cogs/roles/r_create.h
    src/cogs/roles/r_delete.h
    src/cogs/stickers/s_create.h

)

# Add source and header references.

add_executable(Nona ${SOURCES} ${HEADERS})

# Add include directories.

target_include_directories(Nona PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Link libraries provided by vcpkg.

target_link_libraries(Nona PRIVATE

    dpp::dpp
    redis++::redis++_static
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto

)