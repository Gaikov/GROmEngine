cmake_minimum_required(VERSION 3.10)
project(SoundOAL)

file(GLOB sources
        "*.cpp"
        )

add_library(SoundOAL STATIC ${sources})
target_link_libraries(SoundOAL OpenAL SoundBase)

#print_all_variables()