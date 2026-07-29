// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.

#include "ImGUI_metal.h"

#include <GLFW/glfw3.h>

#include "Core/Memory.h"
#include "Engine/RenManager.h"
#include "RasterMetal/nsMetalRenderDevice.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_metal.h"
#include "nsLib/log.h"

bool nsImGUI_metal::BackendInit(void *window) {
    _renderDevice = dynamic_cast<nsMetalRenderDevice *>(nsRenDevice::Shared()->Device());
    if (!_renderDevice || !_renderDevice->GetMetalDevice()) {
        Log::Error("ImGUI Metal backend requires an initialized Metal render device");
        return false;
    }

    _glfwInitialized = ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow *>(window), false);
    if (!_glfwInitialized) {
        return false;
    }

    _rendererInitialized = ImGui_ImplMetal_Init(_renderDevice->GetMetalDevice());
    return _rendererInitialized;
}

void nsImGUI_metal::BackendShutdown() {
    _frameReady = false;

    if (_rendererInitialized) {
        ImGui_ImplMetal_Shutdown();
        _rendererInitialized = false;
    }

    if (_glfwInitialized) {
        ImGui_ImplGlfw_Shutdown();
        _glfwInitialized = false;
    }

    _renderDevice = nullptr;
}

void nsImGUI_metal::BackendStartFrame() {
    nsMemoryLoopAllocScope allocScope;
    _frameReady = false;
    if (!_renderDevice) {
        return;
    }

    _renderDevice->RenderTextureBind(nullptr);

    nsMetalFrameContext context;
    if (!_renderDevice->GetActiveFrameContext(context)) {
        Log::Warning("ImGUI Metal backend has no active render frame");
        ImGui_ImplGlfw_NewFrame();
        return;
    }

    ImGui_ImplMetal_NewFrame(context.renderPassDescriptor);
    ImGui_ImplGlfw_NewFrame();
    _frameReady = true;
}

void nsImGUI_metal::BackendRenderDrawData(ImDrawData *drawData) {
    nsMemoryLoopAllocScope allocScope;
    if (!_frameReady || !_renderDevice) {
        return;
    }

    nsMetalFrameContext context;
    if (_renderDevice->GetActiveFrameContext(context)) {
        ImGui_ImplMetal_RenderDrawData(drawData, context.commandBuffer, context.commandEncoder);
        // Metal has no implicit state restoration; continue engine rendering in a fresh encoder.
        _renderDevice->RenderTextureBind(nullptr);
    }
    _frameReady = false;
}

std::unique_ptr<nsImGUIBackend> CreateImGUIBackend() {
    return std::make_unique<nsImGUI_metal>();
}
