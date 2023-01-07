#ifndef SPECTRAREFX_APP_H
#define SPECTRAREFX_APP_H

#include <vulkan/vulkan_core.h>
#include <memory>
#include "Window.h"
#include "Device.h"
#include "Model.h"

#include "BasicRenderSystem.h"
#include "KeyboardMovementController.h"
#include "Descriptors.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ZERO
#include <glm/vec3.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <glm/detail/type_mat3x3.hpp>
#include "Device.h"
#include "Object.h"
#include "Camera.h"
#include "Render.h"

#include "imguiImports.h"

struct GlobalUBO{
    alignas(16) glm::mat4 projectionView{1.0f};
    alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{3.0f, -5.0f, 1.0f});
};

class App {
public:
    App();
    ~App();
    void run();

    App(const App &) = delete;
    App& operator=(const App &) = delete;

private:
    void createCameraObject();
    void loadObjects();

private:
    Window mainWindow{600, 800, "SpectrareFX"};
    Device device{mainWindow, log};
    Logger log;
    int frame = 0;
    std::vector<Object> objects;
    Render renderer{mainWindow, device};

    std::unique_ptr<lve::LveDescriptorPool> globalPool{};
    std::unique_ptr<Camera> mainCamera;

    std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now();
private:

};

#endif //SPECTRAREFX_APP_H
