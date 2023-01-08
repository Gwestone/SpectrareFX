#include "App.h"
#include "systems/ImGuiRenderSystem.h"

App::App() {
    globalPool = lve::LveDescriptorPool::Builder(device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT * 2)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT * 2)
            .build();
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

    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<Buffer>(
                device,
                sizeof(GlobalUBO),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    auto globalSetLayout = lve::LveDescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .build();

    std::vector<VkDescriptorSet> globalDescriptorSetsList(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSetsList.size(); ++i) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        lve::LveDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSetsList[i]);
    }

    BasicRenderSystem basicRenderSystem{device, renderer.getRenderPass(), globalSetLayout->getDescriptorSetLayout(), log};
    ImGuiRenderSystem imGuiRenderSystem{mainWindow, device, log, renderer.getRenderPass(), globalPool->getDescriptorPool()};

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
            int frameIndex = renderer.getFrameIndex();
            FrameInfo frameInfo{frameIndex, timestep, commandBuffer, *mainCamera, globalDescriptorSetsList[frameIndex]};

            //update
            GlobalUBO ubo{};
            ubo.projectionView = mainCamera->getProjectionMatrix() * mainCamera->getViewMatrix();
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            //render
            renderer.beginRenderPass(commandBuffer);

            basicRenderSystem.renderGameObjects(frameInfo, objects);
            imGuiRenderSystem.renderImGui(frameInfo);

            renderer.endRenderPass(commandBuffer);
            renderer.endFrame();
        }
    }
    vkDeviceWaitIdle(device.getDevice());
}

void App::loadObjects() {
    Object cube{};
    auto model = Model::loadFromFile(device, "./models/viking_room.obj", "./textures/viking_room.png");

    cube.mesh = std::move(model);
    cube.transform.rotation = {glm::half_pi<float>(), 0.0f, 0.0f};
    cube.transform.translation = {0.0f, 0.0f, 0.0f};
    cube.transform.scaleVector = {0.5f, 0.5f, 0.5f};

    objects.push_back(std::move(cube));
}
