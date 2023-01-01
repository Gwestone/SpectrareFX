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

struct PushConstantData{
    glm::vec2 offset;
    glm::mat2 transformation{1.f};
};

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
    void loadModels();
    void recreateSwapChain(uint32_t imageIndex);
    void createSwapChain();
    void freeCommandBuffers();

    void recordCommandBuffer(int imageIndex);
private:
    Window mainWindow{600, 800, "SpectrareFX"};
    Device device{mainWindow, log};
    std::unique_ptr<SwapChain> swapChain;
    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout _pipelineLayout;
    Logger log;
    std::vector<VkCommandBuffer> commandBuffersList;
    std::unique_ptr<Model> model;
    int frame = 0;
};

#endif //SPECTRAREFX_APP_H
