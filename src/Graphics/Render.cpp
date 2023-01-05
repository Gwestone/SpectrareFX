#include "Render.h"

Render::Render(Window& _window, Device& _device) : mainWindow(_window), device(_device) {
    recreateSwapChain();
    createCommandBuffers();
}

Render::~Render() {freeCommandBuffers();}

void Render::recreateSwapChain() {
    auto extent = mainWindow.getExtent();
    while (extent.width == 0 or extent.height == 0){
        extent = mainWindow.getExtent();
        glfwWaitEvents();
    }
    //TODO fix memory leak from resizing window
    vkDeviceWaitIdle(device.getDevice());

    if (swapChain == nullptr){
        swapChain = std::make_unique<SwapChain>(device, log);
    } else{
        swapChain = std::make_unique<SwapChain>(device, log, *std::move(swapChain));
        if (swapChain->imageCount() != commandBuffersList.size()){
            freeCommandBuffers();
            createCommandBuffers();
        }
    }
}

void Render::createCommandBuffers() {
    commandBuffersList.resize(swapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffersList.size());

    if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, commandBuffersList.data()) != VK_SUCCESS){
        throw std::runtime_error("cant allocate new command vertexBuffer");
    }
}

void Render::freeCommandBuffers() {
    vkFreeCommandBuffers(device.getDevice(),
                         device.getCommandPool(),
                         static_cast<uint32_t>(commandBuffersList.size()),
                         commandBuffersList.data());
    commandBuffersList.clear();
}

VkCommandBuffer Render::beginFrame() {

    assert(!isFrameStarted && "cant beginFrame when already in progress");

    float aspectRatio = swapChain->extentAspectRatio();
    //mainCamera->setOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);
    //mainCamera->setProspectiveProjection(1.0f, aspectRatio, 0.1f, 10.0f);
    auto result = swapChain->acquireNextImage(&currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR){
        recreateSwapChain();
        return nullptr;
    }else if (result != VK_SUCCESS and result != VK_SUBOPTIMAL_KHR){
        throw std::runtime_error("cant acquire swap chain image");
    }

    isFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS){
        throw std::runtime_error("cant begin vertexBuffer");
    }

    return commandBuffer;

}

void Render::endFrame() {

    assert(isFrameStarted && "cant end frame when not started");

    auto commandBuffer = getCurrentCommandBuffer();

    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
        throw std::runtime_error("cant submit render commands");
    }

    VkResult result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR or result == VK_SUBOPTIMAL_KHR){
        recreateSwapChain();
    }else if (result != VK_SUCCESS){
        throw std::runtime_error("cant submit images to GPU");
    }

    isFrameStarted = false;
}

void Render::beginRenderPass(VkCommandBuffer _commandBuffer) {
    assert(isFrameStarted && "cant beginRenderPass when already is not in progress");
    assert(getCurrentCommandBuffer() == _commandBuffer && "cant begin render pass on command buffer from different frame");

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = swapChain->getRenderPass();
    renderPassBeginInfo.framebuffer = swapChain->getFrameBuffer(static_cast<int>(currentImageIndex));

    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = swapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

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

    vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);

}

void Render::endRenderPass(VkCommandBuffer _commandBuffer) {
    assert(isFrameStarted && "cant beginRenderPass when already is not in progress");
    assert(getCurrentCommandBuffer() == _commandBuffer && "cant end render pass on command buffer from different frame");

    vkCmdEndRenderPass(_commandBuffer);
}
