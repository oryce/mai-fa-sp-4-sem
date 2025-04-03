add_executable(sp_task2_copy
        copy_file.c)

add_executable(sp_task2_find
        find_string.c)

add_executable(sp_task2
        main.c)
add_dependencies(sp_task2
        sp_task2_copy
        sp_task2_find)