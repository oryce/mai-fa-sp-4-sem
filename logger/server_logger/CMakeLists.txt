add_library(
        mp_os_lggr_srvr_lggr
        src/server_logger.cpp
        src/server_logger_builder.cpp)

add_subdirectory(tests)
find_package(Crow CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(httplib REQUIRED)

target_include_directories(
        mp_os_lggr_srvr_lggr
        PUBLIC
        ./include)
target_link_libraries(
        mp_os_lggr_srvr_lggr
        PUBLIC
        mp_os_cmmn
        Crow::Crow asio::asio)
target_link_libraries(
        mp_os_lggr_srvr_lggr
        PUBLIC
        mp_os_lggr_lggr
        Crow::Crow asio::asio)
target_link_libraries(
        mp_os_lggr_srvr_lggr
        PUBLIC
        nlohmann_json::nlohmann_json
        Crow::Crow asio::asio)
target_link_libraries(
        mp_os_lggr_srvr_lggr
        PRIVATE
        httplib::httplib
        Crow::Crow asio::asio
)


