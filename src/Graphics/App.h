#ifndef SPECTRAREFX_APP_H
#define SPECTRAREFX_APP_H

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

class App {
public:
    App();
    ~App();
    void run();

    App(const App &) = delete;
    App& operator=(const App &) = delete;

private:
    void createPipeline();
    VkPipelineLayout createPipelineLayout();
    void createCommandBuffers();
    void drawFrame();
    void loadObjects();
    void recreateSwapChain(uint32_t imageIndex);
    void createSwapChain();
    void freeCommandBuffers();

    void recordCommandBuffer(int imageIndex);
    void recordRenderObjects(const VkCommandBuffer &_commandBuffer);
private:
    Window mainWindow{600, 800, "SpectrareFX"};
    Device device{mainWindow, log};
    std::unique_ptr<SwapChain> swapChain;
    std::unique_ptr<Pipeline> pipeline;
    static std::unique_ptr<Model> createCubeModel(Device &device, glm::vec3 offset);
    VkPipelineLayout _pipelineLayout;
    Logger log;
    std::vector<VkCommandBuffer> commandBuffersList;
    std::vector<Object> objects;
    int frame = 0;
    std::chrono::milliseconds start;
};

#endif //SPECTRAREFX_APP_H
