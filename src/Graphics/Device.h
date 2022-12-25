#ifndef PARALLEL_DEVICE_H
#define PARALLEL_DEVICE_H

#include <vulkan/vulkan.h>
#include <vector>
#include "Window.h"
#include "DebugLayers.h"
#include "../Logger/Logger.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

class Device {
public:
    Device(const Device &) = delete;
    void operator=(const Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &&) = delete;

    Device(const Window &_window, const Logger &_log);
    VkDevice getDevice(){return device_;};
    SwapChainSupportDetails getSwapChainSupportDetails();
    QueueFamilyIndices findPhysicalQueueFamilies();
    VkPhysicalDevice& getPhysicalDevice();
    VkQueue getGraphicsQueue(){return graphicsQueue;};
    VkQueue getPresentationQueue(){return presentationQueue;};
    const VkSurfaceKHR & getSurface();
    Window & getWindow();
    void createImageWithInfo(
            const VkImageCreateInfo &imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage &image,
            VkDeviceMemory &imageMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkFormat findSupportedFormat( const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

private:
    Logger log;

    VkInstance instance;

    DebugLayers debugLayers;

    VkPhysicalDevice physicalDevice;
    Window window;
    VkCommandPool commandPool;

    VkDevice device_;
    VkSurfaceKHR surface_;
    QueueFamilyIndices queueFamilySetupData;
    std::vector<VkDeviceQueueCreateInfo> populatedQueueFamiliesData;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkQueue graphicsQueue;
    VkQueue presentationQueue;
};

#endif //PARALLEL_DEVICE_H
