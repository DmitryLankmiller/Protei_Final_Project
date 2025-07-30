cmake_minimum_required(VERSION 3.16)
project(mini_pgw)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#     add_compile_options(-Wall -Wextra -Wpedantic -Werror)
# endif()

include(FetchContent)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    http
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG master
)
FetchContent_MakeAvailable(http)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)


set(COMMON_SOURCES
    config/config.cpp
    logger/logger.cpp
)

set(COMMON_INCLUDES
    config/
    logger/
)

add_executable(pgw_server
    pgw-server/main.cpp
    ${COMMON_SOURCES}
    udp-server/udp-server.cpp
    udp-server/message-handler.cpp
    http-server/http-server.cpp
    session/session-manager.cpp
    cdr/cdr-writer.cpp
    blacklist-manager/blacklist-manager.cpp
)

target_include_directories(pgw_server PRIVATE
    ${COMMON_INCLUDES}
    udp-server/
    http-server/
    session/
    cdr/
    blacklist-manager/
)

target_link_libraries(pgw_server
    PRIVATE
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    httplib::httplib
)

add_executable(pgw_client
    pgw-client/main.cpp
    ${COMMON_SOURCES}
    udp-client/udp-client.cpp
)

target_include_directories(pgw_client PRIVATE
    ${COMMON_INCLUDES}
    udp-client/
)

target_link_libraries(pgw_client
    PRIVATE
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    httplib::httplib
)

enable_testing()

add_executable(unit_tests
    ${COMMON_SOURCES}
    tests/test-session-manager.cpp
    tests/test-blacklist-manager.cpp
    udp-server/udp-server.cpp
    udp-server/message-handler.cpp
    http-server/http-server.cpp
    session/session-manager.cpp
    cdr/cdr-writer.cpp
    blacklist-manager/blacklist-manager.cpp
)

target_include_directories(unit_tests PRIVATE
    session/
    config/
    logger/
    cdr/
    udp-server/
    udp-client/
    blacklist-manager/
    tests/
    tests/fixtures/
    tests/mocks/
)

target_link_libraries(unit_tests
    gtest_main
    gmock
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    httplib::httplib
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
