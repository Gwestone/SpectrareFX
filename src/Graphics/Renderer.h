#ifndef PARALLEL_RENDERER_H
#define PARALLEL_RENDERER_H

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "DebugLayers.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"
#include "SyncObjects.h"
#include "Renderer.h"
#include "Window.h"
#include "../Logger/Logger.h"

class Renderer {
public:
    Window window;
    VkInstance instance;
    DebugLayers layers;
    VkSurfaceKHR windowSurface;
    VkPhysicalDevice physicalDevice;
    QueueFamilyIndices deviceQueuesData;
    std::vector<VkDeviceQueueCreateInfo> devicesInfo;
    VkDevice logicalDevice;
    VkQueue graphicsQueue;
    VkQueue presentationQueue;
    VkSwapchainKHR swapChain;
    SwapChainSupportDetails swapChainDetails;
    std::vector<VkImage> swapChainImagesList;
    VkSurfaceFormatKHR surfaceFormat{};
    std::vector<VkImageView> swapChainImageViewList;
    VkExtent2D dimensions{};
    VkRenderPass renderPass;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFrameBuffers;
    std::vector<VkCommandPool> commandPoolsList;
    std::vector<VkCommandBuffer> commandBuffersList;
    SyncObjects sync{};
    Logger log;

    void setupImgui();

public:
    explicit Renderer(const Window &_window);
    void render();
    void wainUntilDeviceIdle() const;
    ~Renderer();
    void cleanSwapChain();
    void recreateSwapChain();
private:
    int currentFrame = 0;
};

#endif //PARALLEL_RENDERER_H
