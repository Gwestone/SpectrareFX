#ifndef SPECTRAREFX_RENDER_H
#define SPECTRAREFX_RENDER_H

#include <vulkan/vulkan_core.h>
#include <memory>
#include "Window.h"
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Model.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ZERO
#include <glm/vec3.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <glm/detail/type_mat3x3.hpp>
#include "Device.h"
#include "Object.h"
#include "Camera.h"

class Render {
public:
    Render(Window& _window, Device& _device);
    ~Render();

    Render(const Render &) = delete;
    Render& operator=(const Render &) = delete;

private:
    void createCommandBuffers();
    void drawFrame();
    void recreateSwapChain();
    void freeCommandBuffers();

public:
    VkRenderPass getRenderPass() const {return swapChain->getRenderPass();}
    bool IsFrameInProcess(){return isFrameStarted;}

    VkCommandBuffer getCurrentCommandBuffer(){
        assert(isFrameStarted && "cant get command buffer when frame isnt started");
        return commandBuffersList[currentImageIndex];
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginRenderPass(VkCommandBuffer _commandBuffer);
    void endRenderPass(VkCommandBuffer _commandBuffer);

    float getAspectRatio(){return swapChain->extentAspectRatio();}

    int getFrameIndex(){
        assert(isFrameStarted && "Cannot get frame index when frame not in progress");
        return currentFrameIndex;
    }

private:
    Window& mainWindow;
    Device& device;
    Logger log;
    std::vector<VkCommandBuffer> commandBuffersList;
    std::unique_ptr<SwapChain> swapChain;

    uint32_t currentImageIndex = 0;
    int currentFrameIndex = 0;
    bool isFrameStarted = false;
};


#endif //SPECTRAREFX_RENDER_H
