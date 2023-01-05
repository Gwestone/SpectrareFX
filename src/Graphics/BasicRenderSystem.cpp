#include "BasicRenderSystem.h"
#include "Camera.h"

BasicRenderSystem::BasicRenderSystem(Device &_device, VkRenderPass renderPass, Logger &_log)
        : device(_device), log(_log) {
    createPipelineLayout();
    createPipeline(renderPass);
}

BasicRenderSystem::~BasicRenderSystem() {
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void BasicRenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void BasicRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::getDefaultPipelineInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayoutInfo = pipelineLayout;
    lvePipeline = std::make_unique<Pipeline>(
            device,
            "shaders/shader.vert.spv",
            "shaders/shader.frag.spv",
            pipelineConfig,
            log);
}

void
BasicRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<Object> &gameObjects, Camera &_camera) {
    lvePipeline->bind(commandBuffer);

    for (auto& obj : gameObjects) {

        PushConstantData push{};
        auto ViewProjection = _camera.getProjectionMatrix() * _camera.getViewMatrix();
        push.transformation = ViewProjection * obj.transform.getTransformationMatrix();

        vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PushConstantData),
                &push);
        obj.mesh->bindDataToBuffer(commandBuffer);
        obj.mesh->drawDataToBuffer(commandBuffer);
    }
}