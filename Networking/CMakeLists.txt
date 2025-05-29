project(Networking)

set(sources
        Net.cpp
        Net.h
        server/SocketServer.cpp
        server/SocketServer.h
        client/ClientSocket.cpp
        client/ClientSocket.h
        server/Server.cpp
        server/Server.h
        Packet.cpp
        Packet.h
)

add_library(Networking STATIC ${sources})
target_link_libraries(Networking PRIVATE ws2_32)