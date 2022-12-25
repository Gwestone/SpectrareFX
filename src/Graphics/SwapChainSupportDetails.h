#ifndef PARALLEL_SWAPCHAINSUPPORTDETAILS_H
#define PARALLEL_SWAPCHAINSUPPORTDETAILS_H


#include <vulkan/vulkan.h>
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    bool isSwapChainAdequate() const{
        return !formats.empty() and !presentModes.empty();
    }
};

#endif //PARALLEL_SWAPCHAINSUPPORTDETAILS_H
