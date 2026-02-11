//
// Created by Roman on 11/12/2025.
//

#include "ImGUI_gles3.h"

#include <GLFW/glfw3.h>

#include "implot.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "nsLib/log.h"

ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode, int scancode);

bool nsImGUI_gles3::Init(void *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
            | ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(window), false);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    return true;
}

void nsImGUI_gles3::Shutdown() {
    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void nsImGUI_gles3::StartFrame() {
    // Начинаем новый кадр ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ShowDockSpace();
}

void nsImGUI_gles3::EndFrame() {
    // Рендеринг
    ImGui::Render();

    // Рендерим ImGui
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool nsImGUI_gles3::OnPointerUp(float x, float y, int pointerId) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseButtonEvent(pointerId, false);
    return io.WantCaptureMouse;
}

bool nsImGUI_gles3::OnPointerDown(float x, float y, int pointerId) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseButtonEvent(pointerId, true);
    return io.WantCaptureMouse;
}

bool nsImGUI_gles3::OnPointerMove(float x, float y, int pointerId) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
    return io.WantCaptureMouse;
}

void nsImGUI_gles3::OnPointerCancel(int pointerId) {
}

void nsImGUI_gles3::OnKeyUp(const int key, const int mods) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddKeyEvent(ImGui_ImplGlfw_KeyToImGuiKey(key, 0), false);

    io.AddKeyEvent(ImGuiMod_Ctrl, (mods & GLFW_MOD_CONTROL) != 0);
    io.AddKeyEvent(ImGuiMod_Shift, (mods & GLFW_MOD_SHIFT) != 0);
    io.AddKeyEvent(ImGuiMod_Alt, (mods & GLFW_MOD_ALT) != 0);
    io.AddKeyEvent(ImGuiMod_Super, (mods & GLFW_MOD_SUPER) != 0);
}

void nsImGUI_gles3::OnKeyDown(const int key, const bool rept, const int mods) {
    if (rept) {
        return;
    }

    ImGuiIO &io = ImGui::GetIO();
    io.AddKeyEvent(ImGui_ImplGlfw_KeyToImGuiKey(key, 0), true);

    io.AddKeyEvent(ImGuiMod_Ctrl, (mods & GLFW_MOD_CONTROL) != 0);
    io.AddKeyEvent(ImGuiMod_Shift, (mods & GLFW_MOD_SHIFT) != 0);
    io.AddKeyEvent(ImGuiMod_Alt, (mods & GLFW_MOD_ALT) != 0);
    io.AddKeyEvent(ImGuiMod_Super, (mods & GLFW_MOD_SUPER) != 0);
}

void nsImGUI_gles3::OnChar(char ch) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(ch);
}

bool nsImGUI_gles3::OnMouseWheel(float delta) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseWheelEvent(0, delta);
    return io.WantCaptureMouse;
}

void nsImGUI_gles3::ShowDockSpace() {

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);

    const ImGuiID spaceId = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(spaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
    ImGui::PopStyleVar();
}
