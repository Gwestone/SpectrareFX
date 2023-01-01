#include "App.h"

App::App() {
    loadModels();
    createSwapChain();
    createPipeline();
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

void App::drawFrame() {
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

void App::loadModels() {
    std::vector<Vertex> vertices = {
            {{0.0f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0}},
            {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0}}
    };

    model = std::make_unique<Model>(device, vertices);

}

void App::recordCommandBuffer(int imageIndex) {

    frame = (frame + 1) % 3000;

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

    pipeline->bind(commandBuffersList[imageIndex]);
    model->bindDataToBuffer(commandBuffersList[imageIndex]);

    for (int i = 0; i < 3; ++i) {
        PushConstantData pushConst{};
        pushConst.offset = {-1.5f + (0.001f * frame), -0.4f + (i * 0.4f)};

        float radToRotate = (frame / 3000.0f) * 6.28;

        pushConst.transformation = Model::rotateTransformationMatrix(pushConst.transformation, radToRotate);



        vkCmdPushConstants(commandBuffersList[imageIndex],
                           _pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(PushConstantData),
                           &pushConst);

        model->drawDataToBuffer(commandBuffersList[imageIndex]);
    }

    vkCmdEndRenderPass(commandBuffersList[imageIndex]);
    if(vkEndCommandBuffer(commandBuffersList[imageIndex]) != VK_SUCCESS){
        throw std::runtime_error("cant submit render commands");
    }
}

void App::freeCommandBuffers() {
    vkFreeCommandBuffers(device.getDevice(),
                         device.getCommandPool(),
                         static_cast<uint32_t>(commandBuffersList.size()),
                         commandBuffersList.data());
    commandBuffersList.clear();
}
