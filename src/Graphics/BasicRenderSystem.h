#ifndef SPECTRAREFX_BASICRENDERSYSTEM_H
#define SPECTRAREFX_BASICRENDERSYSTEM_H

#include <vulkan/vulkan.h>
#include <memory>
#include "Device.h"
#include "Pipeline.h"
#include "Object.h"
#include "Camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ZERO
#include <glm/ext/matrix_float2x2.hpp>

struct PushConstantData{
    glm::mat4 transformation{1.f};
};

class BasicRenderSystem {
public:
    BasicRenderSystem(Device &_device, VkRenderPass renderPass, Logger &_log);
    ~BasicRenderSystem();

    BasicRenderSystem(const BasicRenderSystem &) = delete;
    BasicRenderSystem &operator=(const BasicRenderSystem &) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<Object> &gameObjects, Camera &_camera);

private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    Device &device;
    Logger &log;
    std::unique_ptr<Pipeline> lvePipeline;
    VkPipelineLayout pipelineLayout;
};

#endif //SPECTRAREFX_BASICRENDERSYSTEM_H
