#ifndef SPECTRAREFX_APP_H
#define SPECTRAREFX_APP_H

#include <vulkan/vulkan_core.h>
#include <memory>
#include "Window.h"
#include "Device.h"
#include "Model.h"
#include "BasicRenderSystem.h"

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

    std::unique_ptr<Camera> mainCamera;
private:
    static std::unique_ptr<Model> createCubeModel(Device &device, glm::vec3 offset);
};

#endif //SPECTRAREFX_APP_H
