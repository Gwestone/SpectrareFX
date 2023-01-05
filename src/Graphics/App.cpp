#include "App.h"
#include "BasicRenderSystem.h"
#include "KeyboardMovementController.h"
#include "tiny_obj_loader.h"

App::App() {
    loadObjects();
    createCameraObject();
}

App::~App() {}

void App::createCameraObject() {
    mainCamera = std::make_unique<Camera>();
    //mainCamera->setViewTarget({0.0f, 0.0f, 0.0f}, {0.0f,0.0f,0.0f});
    //mainCamera->setOrthographicProjection(-1, 1, -1, 1, -1, 1);

    //mainCamera->setViewDirection(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //mainCamera->setViewTarget(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f));
}

void App::run() {

    BasicRenderSystem basicRenderSystem{device, renderer.getRenderPass(), log};

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto newTime = std::chrono::high_resolution_clock::now();

    Object ViewerObject{};
    ViewerObject.transform.translation = {0, 0, -1};
    KeyboardMovementController cameraController{};

    while (!mainWindow.shouldClose()) {

        glfwPollEvents();

        newTime = std::chrono::high_resolution_clock::now();
        float timestep = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        cameraController.moveInPlaneXZ(mainWindow, timestep, ViewerObject);
//        mainCamera->setViewYXZ(ViewerObject.transform.translation, ViewerObject.transform.rotation);
        mainCamera->setViewYXZ(ViewerObject.transform.translation, ViewerObject.transform.rotation);
        mainCamera->setProspectiveProjection(glm::radians(50.f), renderer.getAspectRatio(), 0.1f, 10.0f);

        auto commandBuffer = renderer.beginFrame();
        if (commandBuffer != nullptr){
            renderer.beginRenderPass(commandBuffer);
            basicRenderSystem.renderGameObjects(commandBuffer, objects, *mainCamera);
            renderer.endRenderPass(commandBuffer);
            renderer.endFrame();
        }
    }
    vkDeviceWaitIdle(device.getDevice());
}

void App::loadObjects() {
    Object cube{};
    auto model = App::createCubeModel(device, {0.0f, 0.0f, 0.0f});

    cube.mesh = std::move(model);
    cube.transform.rotation = {1.0f, 1.0f, 0.0f};
    cube.transform.translation = {0.0f, 0.0f, 0.0f};
    cube.transform.scaleVector = {.5f, .5f, .5f};

    objects.push_back(std::move(cube));
}

// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<Model> App::createCubeModel(Device &device, glm::vec3 offset) {

    Builder modelBuilder{};
    modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };
    for (auto& v : modelBuilder.vertices) {
        v.position += offset;
    }

    modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                            12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

    return std::make_unique<Model>(device, modelBuilder);
}

