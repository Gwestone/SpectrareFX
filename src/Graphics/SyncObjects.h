#pragma once

#include <vulkan/vulkan.h>

struct SyncObjects{
    std::vector<VkSemaphore> imageAvaiableSemaphoreList;
    std::vector<VkSemaphore> renderFinishedSemaphoreList;
    std::vector<VkFence> inFlightFenceList;
    VkDevice logDevice;
    void destroyObjects(){
        for (VkSemaphore i : imageAvaiableSemaphoreList) {
            vkDestroySemaphore(logDevice, i, nullptr);
        }
        for (VkSemaphore i : renderFinishedSemaphoreList) {
            vkDestroySemaphore(logDevice, i, nullptr);
        }
        for (VkFence i : inFlightFenceList) {
            vkDestroyFence(logDevice, i, nullptr);
        }
    }
};