cmake_minimum_required(VERSION 3.10)
project(SoundBase)

file(GLOB sources
        "*.cpp"
        )

include_directories(
        ./
        GROmEngine/ThirdParty/OggVorbis/include
)

add_library(SoundBase STATIC ${sources})
target_link_libraries(SoundBase OggVorbis)
