cmake_minimum_required(VERSION 3.10)
project(SoundSLES)

file(GLOB sources
        "*.cpp"
        )

add_library(SoundSLES STATIC ${sources})

#print_all_variables()