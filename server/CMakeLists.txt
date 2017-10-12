
add_executable(server
    main.cc
    logger.h
    logger.cc
    tcpserver.h
    tcpserver.cc
    udpserver.h
    udpserver.cc)

target_link_libraries(server PUBLIC
    network
    pthread)
