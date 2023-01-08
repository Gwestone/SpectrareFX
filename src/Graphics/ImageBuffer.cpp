#include "ImageBuffer.h"

ImageBuffer::ImageBuffer(Device &_device, VkImageType imageType, VkMemoryPropertyFlags properties, VkFormat format,
                         VkImageUsageFlags usage, uint32_t width, uint32_t height) : device(_device), format(format) {

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = imageType;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    device.createImageWithInfo(imageInfo, properties, textureImage, textureImageMemory);
//    createTextureImageView();
//    createSampler();
}

void ImageBuffer::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout) {
    device.transitionLayout(textureImage, format, oldLayout, newLayout);
}

void ImageBuffer::createTextureImageView() {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = textureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device.getDevice(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
}

ImageBuffer::~ImageBuffer() {
//    vkDestroySampler(device.getDevice(), textureSampler, nullptr);
//    vkDestroyImageView(device.getDevice(), textureImageView, nullptr);
    vkDestroyImage(device.getDevice(), textureImage, nullptr);
    vkFreeMemory(device.getDevice(), textureImageMemory, nullptr);
}

void ImageBuffer::createSampler() {
//    VkSamplerCreateInfo samplerInfo{};
//    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//    samplerInfo.magFilter = VK_FILTER_LINEAR;
//    samplerInfo.minFilter = VK_FILTER_LINEAR;
//    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//    samplerInfo.anisotropyEnable = VK_TRUE;
//    samplerInfo.maxAnisotropy = device.properties.limits.maxSamplerAnisotropy;
//    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
//    samplerInfo.unnormalizedCoordinates = VK_FALSE;
//    samplerInfo.compareEnable = VK_FALSE;
//    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
//    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//    samplerInfo.mipLodBias = 0.0f;
//    samplerInfo.minLod = 0.0f;
//    samplerInfo.maxLod = 0.0f;
//
//    if (vkCreateSampler(device.getDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS){
//        throw std::runtime_error("cant create sampler");
//    }
}
