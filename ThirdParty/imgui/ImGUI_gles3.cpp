//
// Created by Roman on 11/12/2025.
//

#include "ImGUI_gles3.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

bool nsImGUI_gles3::Init(void *window) {
    // Инициализируем ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Включаем клавиатуру


    // Инициализируем бэкенды ImGui для GLFW и OpenGL ES 3
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(window), false);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    return true;
}

void nsImGUI_gles3::Shutdown() {
    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void nsImGUI_gles3::StartFrame() {
    // Начинаем новый кадр ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void nsImGUI_gles3::EndFrame() {
    // Рендеринг
    ImGui::Render();

    // Рендерим ImGui
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
