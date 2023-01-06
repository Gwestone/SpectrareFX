#include "App.h"

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
    auto model = Model::loadFromFile(device, "./models/colored_cube.obj");

    cube.mesh = std::move(model);
    cube.transform.rotation = {0.0f, 0.0f, 0.0f};
    cube.transform.translation = {0.0f, 0.0f, 0.0f};
    cube.transform.scaleVector = {1.0f, 1.0f, 1.0f};

    objects.push_back(std::move(cube));
}
