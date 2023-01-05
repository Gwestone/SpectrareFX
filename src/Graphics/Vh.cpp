#include <set>
#include <functional>
#include "Vh.h"
#include "../FileHelper.h"
#include "Wrapper.h"
#include "DebugLayers.h"

const std::vector<const char*> Vh::validationLayersList = {
        "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> Vh::requiredExtensionsList = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};

const std::vector<const char*> Vh::requiredDeviceExtensionsList = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

#ifdef NDEBUG
    const bool Vh::enableValidationLayers = false;
#else
    const bool Vh::enableValidationLayers = true;
#endif

VkInstance Vh::createInstance() {

    if (Vh::enableValidationLayers and !Vh::checkIfRequiredValidationLayersSupported()){
        throw std::runtime_error("validation layers requested, but not available!");
    }

    //set basic info for app
    VkApplicationInfo appInfo{};
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "no engine";
    appInfo.pApplicationName = "Vulkan window";
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    //set info for instance
    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    //get and use required extensions for glfw + from required list
    std::vector<const char *> requiredExtensions = Vh::getRequiredExtensions();
    instanceInfo.enabledExtensionCount = requiredExtensions.size();
    instanceInfo.ppEnabledExtensionNames = requiredExtensions.data();

    //use debug layers for instance if layers is enabled
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(Vh::validationLayersList.size());
        instanceInfo.ppEnabledLayerNames = Vh::validationLayersList.data();

        debugCreateInfo = DebugLayers::getDebugLayerInfo();
        instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pNext = nullptr;
    }

    //create instance with avaiablee info
    VkInstance instance;
    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);

    if (result != VK_SUCCESS){
        throw std::runtime_error("failed to create instance!");
    }

    return instance;
}

uint Vh::getExtensionsCount() {

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    return extensionCount;
}

std::vector<VkExtensionProperties> Vh::getExtensionsProperties() {

    uint32_t countOfExtensions = Vh::getExtensionsCount();

    std::vector<VkExtensionProperties> extensions(countOfExtensions);
    VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &countOfExtensions, extensions.data());
    if (res){
        throw std::runtime_error("cant get extensions list");
    }
    return extensions;
}

//check if all required validation layers is supported
bool Vh::checkIfRequiredValidationLayersSupported() {

    //get list of all validation layers supported
    uint32_t countOfLayers = 0;
    vkEnumerateInstanceLayerProperties(&countOfLayers, nullptr);
    std::vector<VkLayerProperties> availableLayers(countOfLayers);

    vkEnumerateInstanceLayerProperties(&countOfLayers, availableLayers.data());

    int countOfFindLayers = 0;

    //compare required and available layers
    for (auto & availableLayer : availableLayers) {
        for (auto & validationLayer : Vh::validationLayersList)
            if (strcmp(availableLayer.layerName, validationLayer) == 0){
                countOfFindLayers += 1;
                break;
            }
    }

    return countOfFindLayers == Vh::validationLayersList.size();
}

std::vector<const char *> Vh::getRequiredExtensions() {

    //get and use required extensions for glfw
    const char** GLFW_requiredExtensions;
    uint32_t countRequiredGLFWExtensions = 0;

    GLFW_requiredExtensions = glfwGetRequiredInstanceExtensions(&countRequiredGLFWExtensions);
    std::vector<const char *> requiredExtensions(GLFW_requiredExtensions,GLFW_requiredExtensions + countRequiredGLFWExtensions);

    //add to required glfw extensions list new extensions
    if (Vh::enableValidationLayers){
        for (auto requiredExtension : Vh::requiredExtensionsList) {
            requiredExtensions.push_back(requiredExtension);
        }
    }

    return requiredExtensions;
}

VkPhysicalDevice Vh::createPhysicalDevice(const VkInstance &instance, const VkSurfaceKHR &windowSurface, const Logger &log) {

    //get count of physical devices
    VkPhysicalDevice device;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    log.printInfo("Detected " + std::to_string(deviceCount) + " devices");

    //get list of all devices
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    //check if device is supported
    for (auto physicalDevice : physicalDevices) {

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        log.printInfo("Detected new device: " + std::string(properties.deviceName));

        if (Vh::ifDeviceSuitable(physicalDevice, windowSurface)){
            device = physicalDevice;
            log.printInfo("Selected device: " + std::string(properties.deviceName));
            break;
        }
    }

    return device;
}

//check if device has geometry shader support and is gpu
bool Vh::ifDeviceSuitable(const VkPhysicalDevice &device, const VkSurfaceKHR &windowSurface) {
    return Vh::findQueueFamilies(device, windowSurface).isComplete()                  //check if device supports graphics and presentation queue
    and Vh::checkIfPhysDeviceSupportRequiredExtensions(device)                        //check if device supports required extensions
    and Vh::querySwapChainSupportDetails(device, windowSurface).isSwapChainAdequate();//check if device supports swap chain
}

QueueFamilyIndices Vh::findQueueFamilies(const VkPhysicalDevice &device, const VkSurfaceKHR &windowSurface) {
    QueueFamilyIndices indices;

    uint32_t countQueueProperties = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &countQueueProperties, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(countQueueProperties);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &countQueueProperties, queueFamilies.data());

    int index = 0;
    for (const auto queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = index;

            VkBool32 surfaceSupport = false;
            VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(device, index, windowSurface, &surfaceSupport);

            if (res != VK_SUCCESS){
                throw std::runtime_error("cant check if surface is supported");
            }
            if (surfaceSupport){
                indices.presentationFamily = index;
            }
        }
        if (indices.isComplete()) {
            break;
        }
        index += 1;
    }

    return indices;
}

VkDevice Vh::createLogicalDevice(const VkPhysicalDevice &physDevice, const QueueFamilyIndices &indices, const std::vector<VkDeviceQueueCreateInfo> &queueCreateInfoList) {

    //set data for logical physDevice
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfoList.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfoList.size());

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(Vh::requiredDeviceExtensionsList.size());
    deviceCreateInfo.ppEnabledExtensionNames = Vh::requiredDeviceExtensionsList.data();

    VkDevice device;
    VkResult result = vkCreateDevice(physDevice, &deviceCreateInfo, nullptr, &device);

    if (result != VK_SUCCESS){
        throw std::runtime_error("cant create logical device");
    }

    return device;
}


VkSurfaceKHR Vh::createWindowSurface(VkInstance const &instance, GLFWwindow *window) {
    VkSurfaceKHR windowSurface;
    VkResult res = glfwCreateWindowSurface(instance, window, nullptr, &windowSurface);
    if (res != VK_SUCCESS){
        throw std::runtime_error("failed to create window surface");
    }
    return windowSurface;
}

std::vector<VkDeviceQueueCreateInfo> Vh::populateQueueCreateInfo(QueueFamilyIndices indices) {

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentationFamily.value()};
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    return queueCreateInfos;
}

VkQueue Vh::createGraphicsQueue(const VkDevice &logDevice, const QueueFamilyIndices &indices) {
    VkQueue graphicsQueue;
    vkGetDeviceQueue(logDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    return graphicsQueue;
}

VkQueue Vh::createPresentationQueue(const VkDevice &logDevice, const QueueFamilyIndices &indices) {
    VkQueue presentationQueue;
    vkGetDeviceQueue(logDevice, indices.presentationFamily.value(), 0, &presentationQueue);
    return presentationQueue;
}

//check if device supports required extensions
bool Vh::checkIfPhysDeviceSupportRequiredExtensions(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr,&extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr,&extensionCount, availableExtensions.data());
    std::set<std::string> requiredExtensions(Vh::requiredDeviceExtensionsList.begin(), Vh::requiredDeviceExtensionsList.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails Vh::querySwapChainSupportDetails(const VkPhysicalDevice &device, const VkSurfaceKHR &windowSurface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, windowSurface, &details.capabilities);

    //querying supported surface formats
    uint32_t surfaceFormatsCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface, &surfaceFormatsCount, nullptr);
    if (surfaceFormatsCount > 0){
        details.formats.resize(surfaceFormatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, windowSurface, &surfaceFormatsCount, details.formats.data());
    }

    //querying supported present modes
    uint32_t presentModesCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface, &presentModesCount, nullptr);
    if (presentModesCount > 0){
        details.presentModes.resize(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, windowSurface, &presentModesCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR Vh::chooseSurfaceFormat(const SwapChainSupportDetails &avaiableFormats) {

    for (auto format : avaiableFormats.formats) {
        if (format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR and format.format == VK_FORMAT_B8G8R8A8_SRGB){
            return format;
        }
    }

    return avaiableFormats.formats[0];
}

VkPresentModeKHR Vh::choosePresentMode(const SwapChainSupportDetails &avaiableFormats, const Logger &log) {
    //TODO solve v_synk jiggering
    for (VkPresentModeKHR mode : avaiableFormats.presentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR){
            log.printInfo("Selected presentation mode: MAILBOX_MODE");
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }
    }
    log.printInfo("Selected presentation mode: IMMEDIATE_MODE");
    return VK_PRESENT_MODE_IMMEDIATE_KHR;
}

VkExtent2D Vh::chooseSwapExtent(const SwapChainSupportDetails &avaiableFormats, GLFWwindow *window) {

    VkSurfaceCapabilitiesKHR capabilities = avaiableFormats.capabilities;
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };
        actualExtent.width = std::clamp(actualExtent.width,
                                        capabilities.minImageExtent.width,
                                        capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                         capabilities.minImageExtent.height,
                                         capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

uint32_t Vh::getSwapChainImageCount(const SwapChainSupportDetails& details) {
    uint32_t imageCount = details.capabilities.minImageCount + 2;
    if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
        imageCount = details.capabilities.maxImageCount;
    }
    return imageCount;
}

VkShaderModule Vh::createShaderModule(const std::vector<char> &shaderBytecode, const VkDevice &logDevice) {
    VkShaderModuleCreateInfo moduleInfo{};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.codeSize = shaderBytecode.size();
    moduleInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBytecode.data());
    VkShaderModule module;
    VkResult res = vkCreateShaderModule(logDevice, &moduleInfo, nullptr, &module);
    if (res != VK_SUCCESS){
        throw std::runtime_error("cant create shader module");
    }
    return module;
}

VkCommandPool Vh::createCommandPool(VkDevice const &logDevice, QueueFamilyIndices indices) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

    VkCommandPool commandPool;
    VkResult res = vkCreateCommandPool(logDevice, &poolInfo, nullptr, &commandPool);
    if (res != VK_SUCCESS){
        throw std::runtime_error("failed to create command pool!");
    }
    return commandPool;
}