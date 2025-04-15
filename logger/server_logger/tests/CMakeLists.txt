find_package(Crow REQUIRED)

add_executable(
        mp_os_lggr_srvr_lggr_tests
        server_logger_tests.cpp)

target_link_libraries(
        mp_os_lggr_srvr_lggr_tests
        PRIVATE
        gtest_main)
target_link_libraries(
        mp_os_lggr_srvr_lggr_tests
        PRIVATE
        mp_os_lggr_srvr_lggr)

add_executable(
        serv_test
        server.cpp
        server.h
        server_test.cpp)

target_link_libraries(
        serv_test
        PRIVATE
        gtest_main)
target_link_libraries(
        serv_test
        PRIVATE
        mp_os_lggr_srvr_lggr)
target_link_libraries(
        serv_test
        PRIVATE
        Crow::Crow)
