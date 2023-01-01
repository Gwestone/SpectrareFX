#include <set>
#include <functional>
#include "Vh.h"
#include "../FileHelper.h"
#include "Wrapper.h"
#include "DebugLayers.h"
#include "Renderer.h"

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

VkSwapchainKHR Vh::createSwapChain(const VkPhysicalDevice &physDevice, const VkSurfaceKHR &windowSurface, GLFWwindow *window,
                    const VkDevice &logDevice, const VkSwapchainKHR &oldSwapchain, const Logger &log) {

    if (oldSwapchain == VK_NULL_HANDLE)
        log.printInfo("Created SwapChain");
    else
        log.printInfo("Recreated SwapChain");

    auto indices = Vh::findQueueFamilies(physDevice, windowSurface);
    SwapChainSupportDetails swapChainDetails = Vh::querySwapChainSupportDetails(physDevice, windowSurface);
    VkSurfaceFormatKHR surfaceFormat = Vh::chooseSurfaceFormat(swapChainDetails);
    VkPresentModeKHR presentMode = Vh::choosePresentMode(swapChainDetails, log);
    VkExtent2D swapChainExtent = Vh::chooseSwapExtent(swapChainDetails, window);

    uint32_t imageCount = Vh::getSwapChainImageCount(swapChainDetails);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = windowSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = swapChainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentationFamily.value()};
    //check if same queue supports graphics command and present commands
    if (indices.graphicsFamily != indices.presentationFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else{
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;// Optional
        createInfo.pQueueFamilyIndices = nullptr;// Optional
    }

    createInfo.preTransform = swapChainDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapchain;

    VkSwapchainKHR swapChain;
    if (vkCreateSwapchainKHR(logDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    return swapChain;
}

uint32_t Vh::getSwapChainImageCount(const SwapChainSupportDetails& details) {
    uint32_t imageCount = details.capabilities.minImageCount + 2;
    if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
        imageCount = details.capabilities.maxImageCount;
    }
    return imageCount;
}

VkPipeline Vh::createGraphicsPipeline(const VkDevice &logDevice, const VkExtent2D &swapExtent,
                                      const VkRenderPass &renderPass) {
    auto fragShaderBytecode = FileHelper::readFile("shaders/shader.frag.spv");
    auto vertShaderBytecode = FileHelper::readFile("shaders/shader.vert.spv");

    VkShaderModule fragModule = Vh::createShaderModule(fragShaderBytecode, logDevice);
    VkShaderModule vertModule = Vh::createShaderModule(vertShaderBytecode, logDevice);

    //create pipeline stage for vertex
    VkPipelineShaderStageCreateInfo vertPipelineInfo{};
    vertPipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertPipelineInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertPipelineInfo.module = vertModule;
    vertPipelineInfo.pName = "main";

    //create pipeline stage for fragment
    VkPipelineShaderStageCreateInfo fragPipelineInfo{};
    fragPipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragPipelineInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragPipelineInfo.module = fragModule;
    fragPipelineInfo.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {vertPipelineInfo, fragPipelineInfo};

    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();


    //describe data to input inside pipeline
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;


    //InputAssembly create info
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapExtent.width;
    viewport.height = (float) swapExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
    rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerInfo.depthClampEnable = VK_FALSE;
    rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerInfo.lineWidth = 1.0f;
    rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizerInfo.depthBiasEnable = VK_FALSE;
    rasterizerInfo.depthBiasConstantFactor = 0.0f; // Optional
    rasterizerInfo.depthBiasClamp = 0.0f; // Optional
    rasterizerInfo.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    VkPipelineLayout pipelineLayout;
    VkResult res = vkCreatePipelineLayout(logDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout);
    if (res != VK_SUCCESS){
        throw std::runtime_error("failed to create pipeline layout!");
    }


    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizerInfo;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    VkPipeline graphicsPipeline;
    res = vkCreateGraphicsPipelines(logDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
    if (res != VK_SUCCESS){
        throw std::runtime_error("failed to create graphics pipeline");
    }

    vkDestroyPipelineLayout(logDevice, pipelineLayout, nullptr);
    vkDestroyShaderModule(logDevice, fragModule, nullptr);
    vkDestroyShaderModule(logDevice, vertModule, nullptr);

    return graphicsPipeline;
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

VkRenderPass Vh::createRenderPass(const VkFormat &renderPassFormat, VkDevice const &logDevice) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = renderPassFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkRenderPass renderPass;
    VkResult res = vkCreateRenderPass(logDevice, &renderPassInfo, nullptr, &renderPass);
    if(res != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

    return renderPass;
}

std::vector<VkFramebuffer> Vh::createFrameBuffers(const VkDevice &logDevice, const std::vector<VkImageView> &swapChainImageViews, const VkExtent2D &extent, const VkRenderPass &renderPass) {

    std::vector<VkFramebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
                swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        VkResult res = vkCreateFramebuffer(logDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
        if (res != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    return swapChainFramebuffers;
}

std::vector<VkCommandPool> Vh::createCommandPoolsList(const VkDevice &logDevice, QueueFamilyIndices indices) {

    std::vector<VkCommandPool> commandPoolsList;
    commandPoolsList.resize(Vh::framesInFlightCount);

    for (int i = 0; i < Vh::framesInFlightCount; ++i) {
        commandPoolsList[i] = Vh::createCommandPool(logDevice, indices);
    }

    return commandPoolsList;
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

std::vector<VkCommandBuffer> Vh::createCommandBuffers(const VkDevice &logDevice, const std::vector<VkCommandPool> &commandPoolsList) {

    std::vector<VkCommandBuffer> commandBuffersList;
    commandBuffersList.resize(Vh::framesInFlightCount);

    for (int i = 0; i < Vh::framesInFlightCount; ++i) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPoolsList[i];
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkResult res = vkAllocateCommandBuffers(logDevice, &allocInfo, &commandBuffersList[i]);
        if (res != VK_SUCCESS){
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    return commandBuffersList;
}

void Vh::recordCommandBuffer(VkCommandBuffer const &commandBuffer, uint32_t imageIndex, const VkRenderPass &renderPass,
                             const std::vector<VkFramebuffer> &swapChainFramebuffers, const VkExtent2D &swapChainExtent,
                             const VkPipeline &graphicsPipeline) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    VkResult res = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (res != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void Vh::drawFrame(const VkDevice &logDevice, const SyncObjects &syncObjects, const VkSwapchainKHR &swapChain,
                   const std::vector<VkCommandBuffer> &commandBuffersList, const VkQueue &graphicsQueue,
                   const VkRenderPass &renderPass, const std::vector<VkFramebuffer> &swapChainFramebuffers,
                   const VkExtent2D &swapChainExtent, const VkPipeline &graphicsPipeline,
                   const VkQueue &presentQueue, int currentFrame, Renderer &renderer) {
    vkWaitForFences(logDevice, 1, &syncObjects.inFlightFenceList[currentFrame], VK_TRUE, UINT64_MAX);

    //TODO make swap chain recreation throught passed parameters
    uint32_t imageIndex;
    VkResult res = vkAcquireNextImageKHR(logDevice, swapChain, UINT64_MAX, syncObjects.imageAvaiableSemaphoreList[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (res == VK_ERROR_OUT_OF_DATE_KHR){
        //recreate swap chain
        renderer.recreateSwapChain();
        return;
    } else if (res != VK_SUCCESS){
        throw std::runtime_error("cant get next frame");
    }

    vkResetFences(logDevice, 1, &syncObjects.inFlightFenceList[currentFrame]);

    vkResetCommandBuffer(commandBuffersList[currentFrame], 0);
    Vh::recordCommandBuffer(commandBuffersList[currentFrame], imageIndex, renderPass, swapChainFramebuffers, swapChainExtent,
                            graphicsPipeline);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {syncObjects.imageAvaiableSemaphoreList[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffersList[currentFrame];

    VkSemaphore signalSemaphores[] = {syncObjects.renderFinishedSemaphoreList[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, syncObjects.inFlightFenceList[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(presentQueue, &presentInfo);

}

SyncObjects Vh::createSyncObjects(VkDevice logDevice) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


    SyncObjects syncObjects{};
    syncObjects.imageAvaiableSemaphoreList.resize(Vh::framesInFlightCount);
    syncObjects.renderFinishedSemaphoreList.resize(Vh::framesInFlightCount);
    syncObjects.inFlightFenceList.resize(Vh::framesInFlightCount);

    for (int i = 0; i < Vh::framesInFlightCount; ++i) {
        if (vkCreateSemaphore(logDevice, &semaphoreInfo, nullptr, &syncObjects.imageAvaiableSemaphoreList[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logDevice, &semaphoreInfo, nullptr, &syncObjects.renderFinishedSemaphoreList[i]) != VK_SUCCESS ||
            vkCreateFence(logDevice, &fenceInfo, nullptr, &syncObjects.inFlightFenceList[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphores!");
        }
    }

    syncObjects.logDevice = logDevice;

    return syncObjects;
}

std::vector<VkImageView> Vh::createImageViewList(VkDevice const &logDevice, const VkFormat &imagesFormat, const std::vector<VkImage> &swapChainImages) {

    std::vector<VkImageView> imageViewList;
    imageViewList.resize(swapChainImages.size());

    for (int i = 0; i < imageViewList.size(); ++i) {
        VkImageViewCreateInfo imageViewInfo{};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.image = swapChainImages[i];
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = imagesFormat;

        imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;

        VkResult res = vkCreateImageView(logDevice, &imageViewInfo, nullptr, &imageViewList[i]);
        if (res != VK_SUCCESS){
            throw std::runtime_error("failed to create image views!");
        }
    }

    return imageViewList;
}

std::vector<VkImage> Vh::createSwapChainImages(const VkDevice &logDevice, const VkSwapchainKHR &swapChain) {

    uint32_t imageCount = 0;
    std::vector<VkImage> swapChainImages;
    vkGetSwapchainImagesKHR(logDevice, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(logDevice, swapChain, &imageCount, swapChainImages.data());

    return swapChainImages;
}
