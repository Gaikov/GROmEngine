project(Networking)

include_directories(
        .
)

set(sources
        Net.cpp
        Net.h
)

add_library(Networking STATIC ${sources})
target_link_libraries(Networking PRIVATE ws2_32)