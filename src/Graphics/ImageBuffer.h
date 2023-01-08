#pragma once

#include <vulkan/vulkan_core.h>
#include "Device.h"

class ImageBuffer {
public:
    ImageBuffer(const ImageBuffer&) = delete;
    ImageBuffer& operator=(const ImageBuffer&) = delete;

    ImageBuffer(Device &_device, VkImageType imageType, VkMemoryPropertyFlags properties, VkFormat format,
                VkImageUsageFlags usage, uint32_t width, uint32_t height);
    ~ImageBuffer();
private:
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    Device &device;
    VkFormat format;
    VkImageView textureImageView;

public:
    VkImage& getImage(){return textureImage;}

public:
    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
    void createTextureImageView();
    void createSampler();

};