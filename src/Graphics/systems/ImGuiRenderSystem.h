#pragma once

#include "../Pipeline.h"
#include "../FrameInfo.h"
#include "../imguiImports.h"
#include "../SwapChain.h"

class ImGuiRenderSystem {
public:
    ImGuiRenderSystem(Window &_window, Device &_device, Logger &_log, VkRenderPass _renderPass,
                      VkDescriptorPool _descriptorPool);
    ~ImGuiRenderSystem();

    ImGuiRenderSystem(const ImGuiRenderSystem &) = delete;
    ImGuiRenderSystem &operator=(const ImGuiRenderSystem &) = delete;

private:
    Window &window;
    Device &device;
    Logger &log;
    std::unique_ptr<Pipeline> lvePipeline;
    VkRenderPass &renderPass;
    VkDescriptorPool &descriptorPool;
    VkCommandPool commandPool;

public:
    void renderImGui(FrameInfo &_frameInfo);

private:
    void initImGui();
    void loadFontTextureAtlas();
    VkCommandBuffer startCommandBuffer();
    void endCommandBuffer(VkCommandBuffer &_commandBuffer);
};
