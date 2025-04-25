# C++ Game Engine
- Based on OpenGL and OpenAL
- Supported platforms: Windows, Linux, MacOS, Android, HTML/WASM
- [WebASM Demo](https://grom-games.com/engine/)

## How to start to make a game
Use [Game Template](https://github.com/Gaikov/GROmGameTemplate)
## Or
Create folders structure like:
```text
GameProject /
    GROmEngine / - GROm engine folder
    GameSources / - Game sources folder
    CMakeLists.txt - cmake file, see example below
```
CMakeLists.txt root example for a game project:

```cmake
project(GROmGameTemplate) #1. change to game name

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_definitions(DEBUG)
endif()

include_directories(
        ./
        GROmEngine
)

add_subdirectory(GROmEngine)
add_subdirectory(GameSources) #2. game source code
```
# Games
- [Friendship's StarWar](https://play.google.com/store/apps/details?id=com.grom.fsw)
- [Castles Keeper](https://play.google.com/store/apps/details?id=com.grom.castleskeeper)

# Links
- [Tiktok](https://www.tiktok.com/@gromgames)
- [Instagram](https://www.instagram.com/gromgamesstudio)
- [Site](https://grom-games.com)

