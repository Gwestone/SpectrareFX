#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <set>
#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/Device.h"

int main() {

    Window window(600, 800, "SpectrareFX");

    Logger log;
    log.setLoggingLevel(LoggingLevels::INFO);
    Device device(window, log);
//    Renderer renderer(window);

//    renderer.setupImgui();

    //main loop
    while(!window.shouldClose()) {
        glfwPollEvents();
//        renderer.render();
    }
//    renderer.wainUntilDeviceIdle();

    return 0;
}