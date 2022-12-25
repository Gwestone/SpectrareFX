#include <cassert>
#include "Pipeline.h"
#include "Vh.h"
#include "../FileHelper.h"

Pipeline::Pipeline(Device &_device, const std::string &vertexFilePath, const std::string &fragmentFilePath,
                   const PipelineConfigInfo &_createInfo, const Logger &_log) : device(_device), log(_log), createInfo(_createInfo) {

    createGraphicsPipeline(vertexFilePath, fragmentFilePath);

}

PipelineConfigInfo Pipeline::getDefaultPipelineInfo(const VkExtent2D &_dimensions) {
    PipelineConfigInfo pipelineInfo{};

    //input assembly config
    pipelineInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    //view port config
    pipelineInfo.viewport.x = 0;
    pipelineInfo.viewport.y = 0;
    pipelineInfo.viewport.width = (float)_dimensions.width;
    pipelineInfo.viewport.height = (float)_dimensions.height;
    pipelineInfo.viewport.minDepth = 0.0f;
    pipelineInfo.viewport.maxDepth = 1.0f;

    //scissors
    pipelineInfo.scissor.offset = {0, 0};
    pipelineInfo.scissor.extent = _dimensions;

    //rasterization info
    pipelineInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    pipelineInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    pipelineInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    pipelineInfo.rasterizationInfo.lineWidth = 1.0f;
    pipelineInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    pipelineInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    pipelineInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    pipelineInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
    pipelineInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
    pipelineInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

    //multisampling
    pipelineInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    pipelineInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipelineInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
    pipelineInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
    pipelineInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
    pipelineInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

    //color blend attachment
    pipelineInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pipelineInfo.colorBlendAttachment.blendEnable = VK_FALSE;
    pipelineInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    pipelineInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    pipelineInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    pipelineInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    pipelineInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    pipelineInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    //color blend config
    pipelineInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    pipelineInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    pipelineInfo.colorBlendInfo.attachmentCount = 1;
    pipelineInfo.colorBlendInfo.pAttachments = &pipelineInfo.colorBlendAttachment;
    pipelineInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
    pipelineInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
    pipelineInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
    pipelineInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

    //depth stencil info
    pipelineInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    pipelineInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    pipelineInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    pipelineInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    pipelineInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    pipelineInfo.depthStencilInfo.minDepthBounds = 0.0f;
    pipelineInfo.depthStencilInfo.maxDepthBounds = 1.0f;
    pipelineInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    pipelineInfo.depthStencilInfo.front = {};
    pipelineInfo.depthStencilInfo.back = {};

    return pipelineInfo;
}

VkPipelineShaderStageCreateInfo Pipeline::createFragmentShader(const std::vector<char> &bytecode) {
    fragmentShader = Vh::createShaderModule(bytecode, device.getDevice());
    //create pipeline stage for vertex
    VkPipelineShaderStageCreateInfo fragmentStageInfo{};
    fragmentStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStageInfo.module = fragmentShader;
    fragmentStageInfo.pName = "main";
    fragmentStageInfo.flags = 0;
    fragmentStageInfo.pNext = nullptr;
    fragmentStageInfo.pSpecializationInfo = nullptr;
    return fragmentStageInfo;
}

VkPipelineShaderStageCreateInfo Pipeline::createVertexShader(const std::vector<char> &bytecode) {
    vertexShader = Vh::createShaderModule(bytecode, device.getDevice());
    //create pipeline stage for vertexShader
    VkPipelineShaderStageCreateInfo vertexStageInfo{};
    vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStageInfo.module = vertexShader;
    vertexStageInfo.pName = "main";
    vertexStageInfo.flags = 0;
    vertexStageInfo.pNext = nullptr;
    vertexStageInfo.pSpecializationInfo = nullptr;
    return vertexStageInfo;
}

void Pipeline::createGraphicsPipeline(const std::string &vertexFilePath, const std::string &fragmentFilePath) {

    assert(createInfo.renderPass != VK_NULL_HANDLE && "cant create graphics pipeline: no renderPass is provided");
    assert(createInfo.pipelineLayoutInfo != VK_NULL_HANDLE && "cant create graphics pipeline: no pipelineLayout is provided");

    auto fragBytecode =  FileHelper::readFile(fragmentFilePath);
    auto vertexBytecode =  FileHelper::readFile(vertexFilePath);

    fragmentShaderStageInfo = createFragmentShader(fragBytecode);
    vertexShaderStageInfo = createVertexShader(vertexBytecode);

    VkPipelineShaderStageCreateInfo shaderStages[2] = {vertexShaderStageInfo, fragmentShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;

    //viewport state
    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &createInfo.viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &createInfo.scissor;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &createInfo.inputAssemblyInfo;
    pipelineCreateInfo.pViewportState = &viewportInfo;
    pipelineCreateInfo.pRasterizationState = &createInfo.rasterizationInfo;
    pipelineCreateInfo.pMultisampleState = &createInfo.multisampleInfo;
    pipelineCreateInfo.pDepthStencilState = &createInfo.depthStencilInfo; // Optional
    pipelineCreateInfo.pColorBlendState = &createInfo.colorBlendInfo;

    pipelineCreateInfo.pDynamicState = nullptr; //need to set

    pipelineCreateInfo.layout = createInfo.pipelineLayoutInfo; //need to set
    pipelineCreateInfo.renderPass = createInfo.renderPass; //need to set
    pipelineCreateInfo.subpass = createInfo.subpass; //need to set

    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineCreateInfo.basePipelineIndex = -1; // Optional


    if (vkCreateGraphicsPipelines(device.getDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS){
        throw std::runtime_error("cant create pipeline");
    }
}

Pipeline::~Pipeline() {
    vkDestroyShaderModule(device.getDevice(), fragmentShader, nullptr);
    vkDestroyShaderModule(device.getDevice(), vertexShader, nullptr);

    vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);
}
