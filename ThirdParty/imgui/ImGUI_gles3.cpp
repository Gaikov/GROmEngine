//
// Created by Roman on 11/12/2025.
//

#include "ImGUI_gles3.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode, int scancode);

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

bool nsImGUI_gles3::OnPointerUp(float x, float y, int pointerId) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(pointerId, false);
    return io.WantCaptureMouse;
}

bool nsImGUI_gles3::OnPointerDown(float x, float y, int pointerId) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(pointerId, true);
    return io.WantCaptureMouse;
}

bool nsImGUI_gles3::OnPointerMove(float x, float y, int pointerId) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
    return io.WantCaptureMouse;
}

void nsImGUI_gles3::OnPointerCancel(int pointerId) {
}

void nsImGUI_gles3::OnKeyUp(int key) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGui_ImplGlfw_KeyToImGuiKey(key, 0), false);
}

void nsImGUI_gles3::OnKeyDown(int key, bool rept) {
    if (rept) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGui_ImplGlfw_KeyToImGuiKey(key, 0), true);

    /*
    io.AddKeyEvent(ImGuiKey_ModCtrl, (mods & GLFW_MOD_CONTROL) != 0);
    io.AddKeyEvent(ImGuiKey_ModShift, (mods & GLFW_MOD_SHIFT) != 0);
    io.AddKeyEvent(ImGuiKey_ModAlt, (mods & GLFW_MOD_ALT) != 0);
    io.AddKeyEvent(ImGuiKey_ModSuper, (mods & GLFW_MOD_SUPER) != 0);
*/
}

void nsImGUI_gles3::OnChar(char ch) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(ch);
}

void nsImGUI_gles3::OnMouseWheel(float delta) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent(0, delta);
}
