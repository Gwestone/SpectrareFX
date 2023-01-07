#include "BasicRenderSystem.h"
#include "Camera.h"

BasicRenderSystem::BasicRenderSystem(Device &_device, VkRenderPass renderPass, VkDescriptorSetLayout _globalDescriptorSetLayout,
                                     Logger &_log)
        : device(_device), log(_log) {
    createPipelineLayout(_globalDescriptorSetLayout);
    createPipeline(renderPass);
}

BasicRenderSystem::~BasicRenderSystem() {
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void BasicRenderSystem::createPipelineLayout(VkDescriptorSetLayout &_globalDescriptorSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayout{_globalDescriptorSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
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

void BasicRenderSystem::renderGameObjects(const FrameInfo &_frameInfo, std::vector<Object> &gameObjects) {
    lvePipeline->bind(_frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(_frameInfo.commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout,
                            0,
                            1,
                            &_frameInfo.globalDescriptorSet,
                            0,
                            nullptr);

    for (auto& obj : gameObjects) {

        PushConstantData push{};

        push.normalMatrix = obj.transform.getNormalMatrix();
        push.modelMatrix = obj.transform.getTransformationMatrixFAST();

        vkCmdPushConstants(
                _frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PushConstantData),
                &push);
        obj.mesh->bindDataToBuffer(_frameInfo.commandBuffer);
        obj.mesh->drawDataToBuffer(_frameInfo.commandBuffer);
    }
}