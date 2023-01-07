#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "../Pipeline.h"
#include "../FrameInfo.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ZERO
#include <glm/ext/matrix_float2x2.hpp>

struct PushConstantData{
    glm::mat4 modelMatrix{1.0f};
    glm::mat4 normalMatrix{1.0f};
};

class BasicRenderSystem {
public:
    BasicRenderSystem(Device &_device, VkRenderPass renderPass, VkDescriptorSetLayout _globalDescriptorSetLayout, Logger &_log);
    ~BasicRenderSystem();

    BasicRenderSystem(const BasicRenderSystem &) = delete;
    BasicRenderSystem &operator=(const BasicRenderSystem &) = delete;

    void renderGameObjects(const FrameInfo &_frameInfo, std::vector<Object> &gameObjects);

private:
    void createPipelineLayout(VkDescriptorSetLayout &_globalDescriptorSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device &device;
    Logger &log;
    std::unique_ptr<Pipeline> lvePipeline;
    VkPipelineLayout pipelineLayout;
};