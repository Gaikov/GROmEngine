project(imgui)

set(sources
        imgui_widgets.cpp
        imgui_tables.cpp
        imgui_draw.cpp
        imgui.cpp
        imgui_demo.cpp
        backends/imgui_impl_glfw.cpp
        backends/imgui_impl_opengl3.cpp
        ImGUI_gles3.cpp
        implot.h
        implot.cpp
        implot_internal.h
        implot_items.cpp
)

include_directories(./)

add_library(imgui
        STATIC
        ${sources}
)

