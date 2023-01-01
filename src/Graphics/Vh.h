#ifndef PARALLEL_VH_H
#define PARALLEL_VH_H

#include <vulkan/vulkan.h>
#include <cstdlib>
#include <vector>
#include "QueueFamilyIndices.h"
#include "../Logger/Logger.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"
#include "SyncObjects.h"
#include "Renderer.h"
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
    static VkSwapchainKHR
    createSwapChain(const VkPhysicalDevice &physDevice, const VkSurfaceKHR &windowSurface, GLFWwindow *window,
                    const VkDevice &logDevice, const VkSwapchainKHR &oldSwapchain, const Logger &log);
    static uint32_t                             getSwapChainImageCount(const SwapChainSupportDetails& details);
    static VkPipeline                           createGraphicsPipeline(const VkDevice &logDevice, const VkExtent2D &swapExtent, const VkRenderPass &renderPass);
    static VkShaderModule                       createShaderModule(const std::vector<char> &shaderBytecode, const VkDevice &logDevice);
    static VkRenderPass                         createRenderPass(const VkFormat &renderPassFormat, const VkDevice &logDevice);
    static std::vector<VkFramebuffer>           createFrameBuffers(const VkDevice &logDevice, const std::vector<VkImageView> &swapChainImageViews, const VkExtent2D &extent, const VkRenderPass &renderPass);
    static std::vector<VkCommandPool>           createCommandPoolsList(const VkDevice &logDevice, QueueFamilyIndices indices);
    static std::vector<VkCommandBuffer>         createCommandBuffers(const VkDevice &logDevice, const std::vector<VkCommandPool> &commandPoolsList);
    static void                                 recordCommandBuffer(VkCommandBuffer const &commandBuffer, uint32_t imageIndex, const VkRenderPass &renderPass,
                                                                    const std::vector<VkFramebuffer> &swapChainFramebuffers, const VkExtent2D &swapChainExtent,
                                                                    const VkPipeline &graphicsPipeline);
    static void                                 drawFrame(const VkDevice &logDevice, const SyncObjects &syncObjects, const VkSwapchainKHR &swapChain,
                                                          const std::vector<VkCommandBuffer> &commandBuffersList, const VkQueue &graphicsQueue,
                                                          const VkRenderPass &renderPass, const std::vector<VkFramebuffer> &swapChainFramebuffers,
                                                          const VkExtent2D &swapChainExtent, const VkPipeline &graphicsPipeline,
                                                          const VkQueue &presentQueue, int currentFrame, Renderer &renderer);
    static SyncObjects                          createSyncObjects(VkDevice logDevice);
    static std::vector<VkImageView>             createImageViewList(const VkDevice &logDevice, const VkFormat &imagesFormat, const std::vector<VkImage> &swapChainImages);
    static std::vector<VkImage>                 createSwapChainImages(const VkDevice &logDevice, const VkSwapchainKHR &swapChain);

    static const bool enableValidationLayers;
    static const std::vector<const char*> validationLayersList;
    static const std::vector<const char*> requiredExtensionsList;
    static const std::vector<const char*> requiredDeviceExtensionsList;
    static const uint32_t framesInFlightCount = 2;

    static VkCommandPool createCommandPool(const VkDevice &logDevice, QueueFamilyIndices indices);
};


#endif //PARALLEL_VH_H
