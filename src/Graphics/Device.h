#pragma once

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

public:

    Device(const Window &_window, const Logger &_log);
    SwapChainSupportDetails getSwapChainSupportDetails();
    QueueFamilyIndices findPhysicalQueueFamilies();
    VkPhysicalDevice& getPhysicalDevice();
    const VkSurfaceKHR & getSurface();
    Window & getWindow();
    void createImageWithInfo(
            const VkImageCreateInfo &imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage &image,
            VkDeviceMemory &imageMemory);
    void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer &buffer,
            VkDeviceMemory &bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkFormat findSupportedFormat( const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkDevice getDevice(){return device_;};
    VkQueue getGraphicsQueue(){return graphicsQueue;};
    VkQueue getPresentationQueue(){return presentationQueue;};
    VkCommandPool getCommandPool(){return commandPool;};
    VkResult copyBuffer(const VkBuffer & _srcBuffer, const VkBuffer & _dstBuffer, VkDeviceSize size);
    VkResult copyBufferToImage(const VkBuffer & _srcBuffer, const VkImage & _dstImage, uint32_t width, uint32_t height);
    VkInstance getInstance(){return instance; }

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

public:
    VkPhysicalDeviceProperties properties;
    VkResult transitionLayout(VkImage _image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
};