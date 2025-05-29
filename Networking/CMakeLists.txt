project(Networking)

set(sources
        Net.cpp
        Net.h
        SocketServer.cpp
        SocketServer.h
        ClientSocket.cpp
        ClientSocket.h
        server/Server.cpp
        server/Server.h
)

add_library(Networking STATIC ${sources})
target_link_libraries(Networking PRIVATE ws2_32)