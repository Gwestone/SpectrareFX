#include "App.h"
#include "Object.h"

App::App() {
    loadObjects();
    createSwapChain();
    createPipeline();
    createCameraObject();
    createCommandBuffers();
}

App::~App() {
    vkDestroyPipelineLayout(device.getDevice(), _pipelineLayout, nullptr);
}

void App::createPipeline() {
    PipelineConfigInfo pipelineConfig{};
    Pipeline::getDefaultPipelineInfo(pipelineConfig);

    _pipelineLayout = createPipelineLayout();

    pipelineConfig.pipelineLayoutInfo = _pipelineLayout;
    pipelineConfig.renderPass = swapChain->getRenderPass();

    pipeline = std::make_unique<Pipeline>(device, "./shaders/shader.vert.spv", "./shaders/shader.frag.spv", pipelineConfig, log);
}

VkPipelineLayout App::createPipelineLayout() {

    VkPushConstantRange constantRange{};
    constantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    constantRange.size = sizeof(PushConstantData);
    constantRange.offset = 0;

    VkPipelineLayout pipelineLayout{};
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
    pipelineLayoutInfo.pPushConstantRanges = &constantRange; // Optional
    if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
        throw std::runtime_error("failed to create pipeline layout!");
    }
    return pipelineLayout;
}

void App::createCommandBuffers() {
    commandBuffersList.resize(swapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffersList.size());

    if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, commandBuffersList.data()) != VK_SUCCESS){
        throw std::runtime_error("cant allocate new command vertexBuffer");
    }

    for (int i = 0; i < commandBuffersList.size(); ++i) {

        recordCommandBuffer(i);

    }
}


void App::createCameraObject() {
    mainCamera = std::make_unique<Camera>();
    //mainCamera->setOrthographicProjection(-1, 1, -1, 1, -1, 1);

    //mainCamera->setViewDirection(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //mainCamera->setViewTarget(glm::vec3(0.0f), glm::vec3(0.0f));
}

void App::drawFrame() {

//    start = std::chrono::duration_cast<std::chrono::milliseconds >(
//            std::chrono::system_clock::now().time_since_epoch()
//    );

    float aspectRatio = swapChain->extentAspectRatio();
    //mainCamera->setOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);
    mainCamera->setProspectiveProjection(1.0f, aspectRatio, 0.1f, 10.0f);

    frame = (frame + 1) % 1000;
    uint32_t imageIndex;
    auto result = swapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR){
        recreateSwapChain(imageIndex);
        return;
    }else if (result != VK_SUCCESS and result != VK_SUBOPTIMAL_KHR){
        throw std::runtime_error("cant acquire swap chain image");
    }

    recordCommandBuffer(imageIndex);
    result = swapChain->submitCommandBuffers(&commandBuffersList[imageIndex], &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR or result == VK_SUBOPTIMAL_KHR){
        recreateSwapChain(imageIndex);
        return;
    }else if (result != VK_SUCCESS){
        throw std::runtime_error("cant submit images to GPU");
    }

//    std::chrono::milliseconds end = std::chrono::duration_cast<std::chrono::milliseconds >(
//            std::chrono::system_clock::now().time_since_epoch()
//    );

//    std::chrono::milliseconds frametime = (end - start);
//
//    log.printInfo("frame time: " + std::to_string(frametime.count()));

}

void App::run() {
    while (!mainWindow.shouldClose()){
        glfwPollEvents();
        drawFrame();
    }
    vkDeviceWaitIdle(device.getDevice());
}

void App::recreateSwapChain(uint32_t imageIndex) {
    auto extent = swapChain->getSwapChainExtent();
    while (extent.width == 0 or extent.height == 0){
        extent = mainWindow.getExtent();
        glfwWaitEvents();
    }
    //TODO fix memory leak from resizing window
    vkDeviceWaitIdle(device.getDevice());

    swapChain.reset();
    pipeline.reset();

    createSwapChain();
    createPipeline();

    freeCommandBuffers();
    createCommandBuffers();
}

void App::createSwapChain() {
    swapChain = std::make_unique<SwapChain>(device, log);
}

void App::loadObjects() {
    Object cube{};
    auto model = App::createCubeModel(device, {0.0f, 0.0f, 0.0f});

    cube.mesh = std::move(model);
    cube.transform.translation = {0.0f, 0.0f, 2.5f};
    cube.transform.scaleVector = {0.5f, 0.5f, 0.5f};

    objects.push_back(std::move(cube));
}

void App::recordCommandBuffer(int imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(commandBuffersList[imageIndex], &beginInfo) != VK_SUCCESS){
        throw std::runtime_error("cant begin vertexBuffer");
    }

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = swapChain->getRenderPass();
    renderPassBeginInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);

    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = swapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffersList[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {swapChain->getSwapChainExtent().width, swapChain->getSwapChainExtent().height};

    vkCmdSetViewport(commandBuffersList[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffersList[imageIndex], 0, 1, &scissor);

//    for (int i = 0; i < 3; ++i) {
//
//    }

    recordRenderObjects(commandBuffersList[imageIndex]);

    vkCmdEndRenderPass(commandBuffersList[imageIndex]);
    if(vkEndCommandBuffer(commandBuffersList[imageIndex]) != VK_SUCCESS){
        throw std::runtime_error("cant submit render commands");
    }
}

void App::recordRenderObjects(VkCommandBuffer const &_commandBuffer) {

    pipeline->bind(_commandBuffer);

    for (auto& object : objects) {
        object.mesh->bindDataToBuffer(_commandBuffer);

//        object.transform.rotation.y = glm::mod(object.transform.rotation.y + 0.0005f, glm::two_pi<float>());
//        object.transform.rotation.x = glm::mod(object.transform.rotation.x + 0.0005f, glm::two_pi<float>());

        glm::mat4 rotationMat(1);
        rotationMat = glm::rotate(rotationMat, mainWindow.getRotation().getX(), glm::vec3(1.0, 0.0, 0.0));
        rotationMat = glm::rotate(rotationMat, mainWindow.getRotation().getY(), glm::vec3(0.0, 1.0, 0.0));

        std::cout << mainWindow.getRotation().getX() << std::endl;
        std::cout << mainWindow.getRotation().getY() << std::endl;

        mainCamera->setViewDirection(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(rotationMat * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));

        PushConstantData pushConst{};
        auto projectionView = mainCamera->getProjectionMatrix() * mainCamera->getViewMatrix();
        pushConst.transformation = projectionView * object.transform.getTransformationMatrix();

        vkCmdPushConstants(_commandBuffer,
                           _pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(PushConstantData),
                           &pushConst);

        object.mesh->drawDataToBuffer(_commandBuffer);
    }

}

void App::freeCommandBuffers() {
    vkFreeCommandBuffers(device.getDevice(),
                         device.getCommandPool(),
                         static_cast<uint32_t>(commandBuffersList.size()),
                         commandBuffersList.data());
    commandBuffersList.clear();
}

// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<Model> App::createCubeModel(Device &device, glm::vec3 offset) {
    std::vector<Vertex> vertices{

            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

    };
    for (auto& v : vertices) {
        v.position += offset;
    }
    return std::make_unique<Model>(device, vertices);
}

