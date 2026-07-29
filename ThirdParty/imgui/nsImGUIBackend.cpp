// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.

#include "nsImGUIBackend.h"

#include <GLFW/glfw3.h>

#include "implot.h"
#include "nsLib/log.h"

ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode, int scancode);

bool nsImGUIBackend::Init(void *window) {
    if (_initialized) {
        return true;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
            | ImGuiConfigFlags_DockingEnable;

    if (!BackendInit(window)) {
        BackendShutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
        return false;
    }

    _initialized = true;
    return true;
}

void nsImGUIBackend::Shutdown() {
    if (!_initialized) {
        Log::Warning("ImGUI was not initialized");
        return;
    }

    BackendShutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    _initialized = false;
}

void nsImGUIBackend::StartFrame() {
    if (!_initialized) {
        return;
    }

    BackendStartFrame();
    ImGui::NewFrame();
    ShowDockSpace();
}

void nsImGUIBackend::EndFrame() {
    if (!_initialized) {
        return;
    }

    ImGui::Render();
    BackendRenderDrawData(ImGui::GetDrawData());
}

bool nsImGUIBackend::OnPointerUp(float x, float y, int pointerId) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseButtonEvent(pointerId, false);
    return io.WantCaptureMouse;
}

bool nsImGUIBackend::OnPointerDown(float x, float y, int pointerId) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseButtonEvent(pointerId, true);
    return io.WantCaptureMouse;
}

bool nsImGUIBackend::OnPointerMove(float x, float y, int pointerId) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
    return io.WantCaptureMouse;
}

void nsImGUIBackend::OnPointerCancel(int pointerId) {
}

void nsImGUIBackend::OnKeyUp(const int key, const int mods) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddKeyEvent(ImGui_ImplGlfw_KeyToImGuiKey(key, 0), false);

    io.AddKeyEvent(ImGuiMod_Ctrl, (mods & GLFW_MOD_CONTROL) != 0);
    io.AddKeyEvent(ImGuiMod_Shift, (mods & GLFW_MOD_SHIFT) != 0);
    io.AddKeyEvent(ImGuiMod_Alt, (mods & GLFW_MOD_ALT) != 0);
    io.AddKeyEvent(ImGuiMod_Super, (mods & GLFW_MOD_SUPER) != 0);
}

void nsImGUIBackend::OnKeyDown(const int key, const bool repeat, const int mods) {
    if (repeat) {
        return;
    }

    ImGuiIO &io = ImGui::GetIO();
    io.AddKeyEvent(ImGui_ImplGlfw_KeyToImGuiKey(key, 0), true);

    io.AddKeyEvent(ImGuiMod_Ctrl, (mods & GLFW_MOD_CONTROL) != 0);
    io.AddKeyEvent(ImGuiMod_Shift, (mods & GLFW_MOD_SHIFT) != 0);
    io.AddKeyEvent(ImGuiMod_Alt, (mods & GLFW_MOD_ALT) != 0);
    io.AddKeyEvent(ImGuiMod_Super, (mods & GLFW_MOD_SUPER) != 0);
}

void nsImGUIBackend::OnChar(char ch) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(ch);
}

bool nsImGUIBackend::OnMouseWheel(float delta) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseWheelEvent(0, delta);
    return io.WantCaptureMouse;
}

void nsImGUIBackend::ShowDockSpace() {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("DockSpace Demo", nullptr, windowFlags);

    const ImGuiID spaceId = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(spaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
    ImGui::PopStyleVar();
}
