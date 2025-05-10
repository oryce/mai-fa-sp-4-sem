add_executable(mp_os_arthmtc_frctn_test test.cpp)

target_link_libraries(
        mp_os_arthmtc_frctn_test
        PRIVATE
        gtest_main
)

target_link_libraries(
        mp_os_arthmtc_frctn_test
        PRIVATE
        mp_os_arthmtc_frctn
)
target_link_libraries(
        mp_os_arthmtc_frctn_test
        PUBLIC
        mp_os_cmmn
        mp_os_allctr_allctr
        mp_os_arthmtc_bg_intgr
        mp_os_lggr_clnt_lggr
)