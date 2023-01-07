#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class DebugLayers {
private:
    std::vector<VkDebugUtilsMessengerEXT> debugLayersList;
    VkInstance instance;
    VkDebugUtilsMessengerEXT createDebugLayer();
public:
    static VkDebugUtilsMessengerCreateInfoEXT getDebugLayerInfo();
    DebugLayers(const VkInstance &_instance);
    DebugLayers();
    ~DebugLayers();
    void destroyDebugLayers();
    //callbacks
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};