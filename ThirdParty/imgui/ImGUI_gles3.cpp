//
// Created by Roman on 11/12/2025.
//

#include "ImGUI_gles3.h"

#include <GLFW/glfw3.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

bool nsImGUI_gles3::BackendInit(void *window) {
    _glfwInitialized = ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(window), false);
    if (!_glfwInitialized) {
        return false;
    }

    _rendererInitialized = ImGui_ImplOpenGL3_Init("#version 300 es");
    return _rendererInitialized;
}

void nsImGUI_gles3::BackendShutdown() {
    if (_rendererInitialized) {
        ImGui_ImplOpenGL3_Shutdown();
        _rendererInitialized = false;
    }

    if (_glfwInitialized) {
        ImGui_ImplGlfw_Shutdown();
        _glfwInitialized = false;
    }
}

void nsImGUI_gles3::BackendStartFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void nsImGUI_gles3::BackendRenderDrawData(ImDrawData *drawData) {
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
}

std::unique_ptr<nsImGUIBackend> CreateImGUIBackend() {
    return std::make_unique<nsImGUI_gles3>();
}
