#include "Renderer.h"
#include "Window.h"
#include "Vh.h"

//1. create window
//2. create instance
//3. (Optional) create debug layers
//3. create window surface
//4. create physical device
//5. find graphical and presentational queues data
//6. create logical device
//7. create graphical and presentation queue
//8. create swap chain and get SwapChainDetails
//9. get images from swap chain
//10. get surface format of swapChain
//11. get imageViews list of swapChain
//12. create render pass
//13. create graphics pipeline
//14. get swap chain frame buffers
//15. create command pool and command buffer
//16. create sync objects for render

Renderer::Renderer(const Window &_window) : window(_window) {

    log.setLoggingLevel(LoggingLevels::INFO);

    //start device
    //init vulkan primitives
    instance = Vh::createInstance();

    layers = DebugLayers(instance);

    windowSurface = Vh::createWindowSurface(instance, window.getWindowObj());
    physicalDevice = Vh::createPhysicalDevice(instance, windowSurface, log);
    deviceQueuesData = Vh::findQueueFamilies(physicalDevice, windowSurface);
    devicesInfo = Vh::populateQueueCreateInfo(deviceQueuesData);
    logicalDevice = Vh::createLogicalDevice(physicalDevice, deviceQueuesData, devicesInfo);

    graphicsQueue = Vh::createGraphicsQueue(logicalDevice, deviceQueuesData);
    presentationQueue = Vh::createPresentationQueue(logicalDevice, deviceQueuesData);
    //end device

    //start swapChain
    swapChain = Vh::createSwapChain(physicalDevice, windowSurface, window.getWindowObj(), logicalDevice, VK_NULL_HANDLE, log);
    swapChainDetails = Vh::querySwapChainSupportDetails(physicalDevice, windowSurface);
    swapChainImagesList = Vh::createSwapChainImages(logicalDevice, swapChain);
    surfaceFormat = Vh::chooseSurfaceFormat(swapChainDetails);
    swapChainImageViewList = Vh::createImageViewList(logicalDevice, surfaceFormat.format, swapChainImagesList);

    dimensions = Vh::chooseSwapExtent(swapChainDetails, window.getWindowObj());
    renderPass = Vh::createRenderPass(surfaceFormat.format, logicalDevice);

    //start pipeline
    graphicsPipeline = Vh::createGraphicsPipeline(logicalDevice, dimensions, renderPass);
    //end pipeline

    swapChainFrameBuffers = Vh::createFrameBuffers(logicalDevice, swapChainImageViewList, dimensions, renderPass);
    //end swap chain

    commandPoolsList = Vh::createCommandPoolsList(logicalDevice, deviceQueuesData);
    commandBuffersList = Vh::createCommandBuffers(logicalDevice, commandPoolsList);

    sync = Vh::createSyncObjects(logicalDevice);
}

void Renderer::render() {
    using namespace std::chrono;
    milliseconds start = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    );
    currentFrame = (currentFrame + 1) % (int)Vh::framesInFlightCount;
    Vh::drawFrame(logicalDevice, sync, swapChain, commandBuffersList, graphicsQueue, renderPass, swapChainFrameBuffers,
                  dimensions, graphicsPipeline, presentationQueue, currentFrame, *this);

    milliseconds end = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    );

    milliseconds frametime = (end - start);

//    log.printInfo("frame time: " + std::to_string(frametime.count()));

}

void Renderer::wainUntilDeviceIdle() const {
    vkDeviceWaitIdle(logicalDevice);
}

Renderer::~Renderer() {
    sync.destroyObjects();
    cleanSwapChain();
    for (auto & i : commandPoolsList) {
        vkDestroyCommandPool(logicalDevice, i, nullptr);
    }
    vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
    vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
    vkDestroySurfaceKHR(instance, windowSurface, nullptr);
    layers.destroyDebugLayers();
    vkDestroyDevice(logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void Renderer::cleanSwapChain() {
    for (VkImageView imageView : swapChainImageViewList) {
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }
    for (VkFramebuffer framebuffer : swapChainFrameBuffers) {
        vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
    }
    vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
}

void Renderer::recreateSwapChain() {
    vkDeviceWaitIdle(logicalDevice);

    cleanSwapChain();

//    renderPass = Vh::createRenderPass(surfaceFormat, logicalDevice);
    swapChain = Vh::createSwapChain(physicalDevice, windowSurface, window.getWindowObj(), logicalDevice, VK_NULL_HANDLE, log);
    swapChainImagesList = Vh::createSwapChainImages(logicalDevice, swapChain);
    swapChainImageViewList = Vh::createImageViewList(logicalDevice, surfaceFormat.format, swapChainImagesList);
    swapChainFrameBuffers = Vh::createFrameBuffers(logicalDevice, swapChainImageViewList, dimensions, renderPass);
}

void Renderer::setupImgui() {

//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO();
//    ImGui::StyleColorsDark();
//    // Setup Platform/Renderer backends
//    ImGui_ImplGlfw_InitForVulkan(window, true);
}
