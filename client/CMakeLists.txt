
add_executable(client
    main.cc)

target_link_libraries(client PUBLIC
    network)
