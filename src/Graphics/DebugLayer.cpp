#include "DebugLayers.h"
#include "Vh.h"
#include "Wrapper.h"

DebugLayers::DebugLayers(const VkInstance &_instance) : instance(_instance) {
    debugLayersList.push_back(createDebugLayer());
}

DebugLayers::DebugLayers() {

}

DebugLayers::~DebugLayers() {}

void DebugLayers::destroyDebugLayers() {
    for (VkDebugUtilsMessengerEXT i : debugLayersList) {
        Wrapper::DestroyDebugUtilsMessengerEXT(instance, i, nullptr);
    }
}

VkDebugUtilsMessengerEXT DebugLayers::createDebugLayer() {
    VkDebugUtilsMessengerCreateInfoEXT debuggerInfo = DebugLayers::getDebugLayerInfo();

    VkDebugUtilsMessengerEXT debugLayer;
    VkResult result = Wrapper::CreateDebugUtilsMessengerEXT(instance, &debuggerInfo, nullptr, &debugLayer);

    if(result != VK_SUCCESS){
        throw std::runtime_error("failed to set up debug messenger!");
    }

    return debugLayer;
}

VkDebugUtilsMessengerCreateInfoEXT DebugLayers::getDebugLayerInfo() {
    VkDebugUtilsMessengerCreateInfoEXT debuggerInfo{};

    debuggerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debuggerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    debuggerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debuggerInfo.pfnUserCallback = DebugLayers::debugCallback;
    debuggerInfo.pUserData = nullptr; // Optional

    return debuggerInfo;
}

VkBool32 DebugLayers::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    Logger log;
    log.setLoggingLevel(LoggingLevels::INFO);
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            log.printInfo(pCallbackData->pMessage);
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            log.printWarn(pCallbackData->pMessage);
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            log.printError(pCallbackData->pMessage);
    }
    return VK_FALSE;
}

