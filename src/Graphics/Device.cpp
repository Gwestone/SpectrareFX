#include "Device.h"
#include "Vh.h"

Device::Device(const Window &_window, const Logger &_log) : window(_window), log(_log) {
    instance = Vh::createInstance();

    debugLayers = DebugLayers(instance);

    surface_ = Vh::createWindowSurface(instance, window.getWindowObj());
    physicalDevice = Vh::createPhysicalDevice(instance, surface_, log);
    queueFamilySetupData = Vh::findQueueFamilies(physicalDevice, surface_);
    populatedQueueFamiliesData = Vh::populateQueueCreateInfo(queueFamilySetupData);
    device_ = Vh::createLogicalDevice(physicalDevice, queueFamilySetupData, populatedQueueFamiliesData);
    commandPool = Vh::createCommandPool(device_, queueFamilySetupData);

    graphicsQueue = Vh::createGraphicsQueue(device_, queueFamilySetupData);
    presentationQueue = Vh::createPresentationQueue(device_, queueFamilySetupData);
}

SwapChainSupportDetails Device::getSwapChainSupportDetails() {
    return Vh::querySwapChainSupportDetails(physicalDevice, surface_);
}

VkPhysicalDevice &Device::getPhysicalDevice() {
    return physicalDevice;
}

VkSurfaceKHR const & Device::getSurface() {
    return surface_;
}

Window & Device::getWindow() {
    return window;
}

QueueFamilyIndices Device::findPhysicalQueueFamilies() {
    return Vh::findQueueFamilies(physicalDevice, surface_);
}

void Device::createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image,
                                 VkDeviceMemory &imageMemory) {
    if (vkCreateImage(device_, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device_, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    if (vkBindImageMemory(device_, image, imageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

VkFormat Device::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (
                tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
}
