#include "ImGuiRenderSystem.h"

ImGuiRenderSystem::ImGuiRenderSystem(Window &_window, Device &_device, Logger &_log, VkRenderPass _renderPass,
                                     VkDescriptorPool _descriptorPool) : window(_window), device(_device), log(_log), renderPass(_renderPass), descriptorPool(_descriptorPool) {
    initImGui();
    loadFontTextureAtlas();
}

ImGuiRenderSystem::~ImGuiRenderSystem() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void ImGuiRenderSystem::initImGui() {
    //init imgui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.Fonts->AddFontFromFileTTF("./fonts/Roboto-Medium.ttf", 18);

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window.getWindowObj(), true);

    ImGui_ImplVulkan_InitInfo guiInitInfo{};
    guiInitInfo.Instance = device.getInstance();
    guiInitInfo.Queue = device.getGraphicsQueue();
    guiInitInfo.DescriptorPool = descriptorPool;
    guiInitInfo.Device = device.getDevice();
    guiInitInfo.PhysicalDevice = device.getPhysicalDevice();
    guiInitInfo.ImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
    guiInitInfo.MinImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
//    guiInitInfo.MSAASamples = msaaSample;
    ImGui_ImplVulkan_Init(&guiInitInfo, renderPass);
    //end init imgui
}

void ImGuiRenderSystem::renderImGui(FrameInfo &_frameInfo) {
    //make draw gui function
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _frameInfo.commandBuffer);
}

void ImGuiRenderSystem::loadFontTextureAtlas() {
    //create font texture atlas for imgui
    auto commandBuffer = startCommandBuffer();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    endCommandBuffer(commandBuffer);
    vkDeviceWaitIdle(device.getDevice());
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

VkCommandBuffer ImGuiRenderSystem::startCommandBuffer() {

    //alloc pool fot buffers
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = device.findPhysicalQueueFamilies().graphicsFamily.value();
    if (vkCreateCommandPool(device.getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }

    //alloc single time buffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device.getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void ImGuiRenderSystem::endCommandBuffer(VkCommandBuffer &_commandBuffer) {

    assert(commandPool && "cant end command buffer without start");

    if(vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS){
        throw std::runtime_error("cant submit render commands");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer;

    vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.getGraphicsQueue());

    vkFreeCommandBuffers(device.getDevice(), commandPool, 1, &_commandBuffer);
}
