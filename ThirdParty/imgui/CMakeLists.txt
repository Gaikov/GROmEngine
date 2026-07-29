project(imgui)

set(sources
        imgui_widgets.cpp
        imgui_tables.cpp
        imgui_draw.cpp
        imgui.cpp
        imgui_demo.cpp
        backends/imgui_impl_glfw.cpp
        nsImGUIBackend.cpp
        nsImGUIBackend.h
        implot.h
        implot.cpp
        implot_internal.h
        implot_items.cpp
)

if (APPLE)
    list(APPEND sources
            backends/imgui_impl_metal.mm
            ImGUI_metal.h
            ImGUI_metal.mm
    )
else ()
    list(APPEND sources
            backends/imgui_impl_opengl3.cpp
            ImGUI_gles3.h
            ImGUI_gles3.cpp
    )
endif ()

include_directories(./)

add_library(imgui
        STATIC
        ${sources}
)
if (NOT WEB_ASM)
    find_package(glfw3 REQUIRED)
    target_link_libraries(imgui glfw)
    if(TARGET glfw)
        get_target_property(GLFW_INC glfw INTERFACE_INCLUDE_DIRECTORIES)
        if(GLFW_INC)
            target_include_directories(imgui PRIVATE ${GLFW_INC})
        endif()
    endif()
endif ()

if (APPLE)
    find_library(IMGUI_METAL_LIB Metal REQUIRED)
    find_library(IMGUI_QUARTZCORE_LIB QuartzCore REQUIRED)
    find_library(IMGUI_APPKIT_LIB AppKit REQUIRED)
    target_link_libraries(imgui
            RasterMetal
            ${IMGUI_METAL_LIB}
            ${IMGUI_QUARTZCORE_LIB}
            ${IMGUI_APPKIT_LIB}
    )
    set_source_files_properties(
            backends/imgui_impl_metal.mm
            ImGUI_metal.mm
            PROPERTIES COMPILE_FLAGS "-fobjc-arc"
    )
endif ()
