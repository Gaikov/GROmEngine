cmake_minimum_required(VERSION 3.10)
project(functional_tests)

set(SRC
        AppInit.cpp
        tests/Sprite3dTest.cpp
        tests/PlaneSprite3dTest.cpp
        TestGameApp.cpp
        SoundTest.cpp
        tests/QuadsTest.cpp
        tests/sprites/Sprite9GridTest.cpp
        tests/GlobalToLocalTest.cpp
        tests/GlobalToLocalTest.h
        tests/layouts/LayoutsTest.cpp
        tests/layouts/LayoutsTest.h
        tests/rays/RayMarchingTest.cpp
        tests/rays/RayMarchingTest.h
        tests/rays/RayBoxTest.cpp
        tests/rays/RayBoxTest.h
        tests/rays/RayLightTest.cpp
        tests/rays/RayLightTest.h
        tests/rays/RayMarching.cpp
        tests/rays/RayMarching.h
        tests/StencilMaskTest.cpp
        tests/StencilMaskTest.h
        tests/layouts/ScrollTest.cpp
        tests/layouts/ScrollTest.h
        tests/sprites/ParticlesTest.cpp
        tests/sprites/ParticlesTest.h
        tests/FontTest.cpp
        tests/LinesTest.cpp
        tests/SpriteTest.cpp
)

if (ANDROID)
    add_library(functional_tests STATIC ${sources} ${SRC})
else ()
    add_executable(functional_tests ${sources} ${SRC})
endif ()

include_directories(./)

target_link_libraries(
        functional_tests
        ${GROM_ENGINE_LIBS}
)
