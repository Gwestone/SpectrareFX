#ifndef SPECTRAREFX_FRAMEINFO_H
#define SPECTRAREFX_FRAMEINFO_H

#include "Camera.h"
#include <vulkan/vulkan.h>

struct FrameInfo{
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera &camera;
    VkDescriptorSet &globalDescriptorSet;
};

#endif //SPECTRAREFX_FRAMEINFO_H
