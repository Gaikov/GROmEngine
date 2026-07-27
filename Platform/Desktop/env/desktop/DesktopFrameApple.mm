#include "Engine/engine.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void DesktopRunFrame() {
    @autoreleasepool {
        nsEngine::MainLoop();
        glfwPollEvents();
    }
}
