#pragma once

#include <vulkan/vulkan.h>
#include <cstdlib>
#include <vector>
#include "QueueFamilyIndices.h"
#include "../Logger/Logger.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"
#include "SyncObjects.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <cstring>

class Vh {
public:
    static uint                                 getExtensionsCount();
    static VkInstance                           createInstance();
    static std::vector<VkExtensionProperties>   getExtensionsProperties();
    static bool                                 checkIfRequiredValidationLayersSupported();
    static std::vector<const char *>            getRequiredExtensions();
    static VkDebugUtilsMessengerEXT             setupDebugger(const VkInstance &instance);
    static VkPhysicalDevice
    createPhysicalDevice(const VkInstance &instance, const VkSurfaceKHR &windowSurface, const Logger &log);
    static bool                                 ifDeviceSuitable(const VkPhysicalDevice &device,const VkSurfaceKHR &windowSurface);
    static QueueFamilyIndices                   findQueueFamilies(const VkPhysicalDevice &device,const VkSurfaceKHR &windowSurface);
    static VkDevice                             createLogicalDevice(const VkPhysicalDevice &physDevice, const QueueFamilyIndices &indices, const std::vector<VkDeviceQueueCreateInfo> &queueCreateInfoList);
    static VkQueue                              createGraphicsQueue(const VkDevice &logDevice, const QueueFamilyIndices &indices);
    static VkSurfaceKHR                         createWindowSurface(VkInstance const &instance, GLFWwindow *window);
    static std::vector<VkDeviceQueueCreateInfo> populateQueueCreateInfo(QueueFamilyIndices indices);
    static VkQueue                              createPresentationQueue(VkDevice const &logDevice, const QueueFamilyIndices &indices);
    static bool                                 checkIfPhysDeviceSupportRequiredExtensions(VkPhysicalDevice device);
    static SwapChainSupportDetails              querySwapChainSupportDetails(const VkPhysicalDevice &device, const VkSurfaceKHR &windowSurface);
    static VkSurfaceFormatKHR                   chooseSurfaceFormat(const SwapChainSupportDetails &avaiableFormats);
    static VkPresentModeKHR choosePresentMode(const SwapChainSupportDetails &avaiableFormats, const Logger &log);
    static VkExtent2D                           chooseSwapExtent(const SwapChainSupportDetails &avaiableFormats, GLFWwindow *window);
    static uint32_t                             getSwapChainImageCount(const SwapChainSupportDetails& details);
    static VkShaderModule                       createShaderModule(const std::vector<char> &shaderBytecode, const VkDevice &logDevice);

    static const bool enableValidationLayers;
    static const std::vector<const char*> validationLayersList;
    static const std::vector<const char*> requiredExtensionsList;
    static const std::vector<const char*> requiredDeviceExtensionsList;
    static const uint32_t framesInFlightCount = 2;

    static VkCommandPool createCommandPool(const VkDevice &logDevice, QueueFamilyIndices indices);
};