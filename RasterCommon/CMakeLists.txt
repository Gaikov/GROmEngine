add_library(RasterCommon STATIC DisplayModeConfig.cpp)
target_include_directories(RasterCommon PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(RasterCommon PRIVATE Core nsLib)
