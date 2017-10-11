
add_library(network STATIC
    networkexception.h
    networkexception.cc
    internetaddress.h
    internetaddress.cc
    socket.h
    socket.cc
    tcpsocket.h
    tcpsocket.cc
    tcpserversocket.h
    tcpserversocket.cc
    udpsocket.h
    udpsocket.cc)
add_library(protei::network ALIAS network)

target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

