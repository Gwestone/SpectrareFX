#ifndef PARALLEL_PIPELINE_H
#define PARALLEL_PIPELINE_H

#include <string>
#include "Device.h"

struct PipelineConfigInfo {
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayoutInfo = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

class Pipeline {
public:
    Pipeline(Device &_device, const std::string &vertexFilePath, const std::string &fragmentFilePath,
             const PipelineConfigInfo &_createInfo, const Logger &_log);
    ~Pipeline();
    static PipelineConfigInfo getDefaultPipelineInfo(const VkExtent2D &_dimensions);
private:
    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo;
    VkShaderModule fragmentShader;

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo;
    VkShaderModule vertexShader;

    PipelineConfigInfo createInfo;
    Device& device;
    Logger log;
    VkPipeline graphicsPipeline;
    VkPipelineShaderStageCreateInfo createFragmentShader(const std::vector<char> &bytecode);
    VkPipelineShaderStageCreateInfo createVertexShader(const std::vector<char> &bytecode);

    void createGraphicsPipeline(const std::string &vertexFilePath, const std::string &fragmentFilePath);
};

#endif //PARALLEL_PIPELINE_H
