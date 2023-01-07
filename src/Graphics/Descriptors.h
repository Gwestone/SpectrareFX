#pragma once

#include "Device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace lve {

    class LveDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(Device &lveDevice) : lveDevice{lveDevice} {}

            Builder &addBinding(
                    uint32_t binding,
                    VkDescriptorType descriptorType,
                    VkShaderStageFlags stageFlags,
                    uint32_t count = 1);
            std::unique_ptr<LveDescriptorSetLayout> build() const;

        private:
            Device &lveDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        LveDescriptorSetLayout(
                Device &lveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~LveDescriptorSetLayout();
        LveDescriptorSetLayout(const LveDescriptorSetLayout &) = delete;
        LveDescriptorSetLayout &operator=(const LveDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        Device &lveDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class LveDescriptorWriter;
    };

    class LveDescriptorPool {
    public:
        class Builder {
        public:
            Builder(Device &lveDevice) : lveDevice{lveDevice} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<LveDescriptorPool> build() const;

        private:
            Device &lveDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        LveDescriptorPool(
                Device &lveDevice,
                uint32_t maxSets,
                VkDescriptorPoolCreateFlags poolFlags,
                const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~LveDescriptorPool();
        LveDescriptorPool(const LveDescriptorPool &) = delete;
        LveDescriptorPool &operator=(const LveDescriptorPool &) = delete;

        bool allocateDescriptor(
                const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;
        VkDescriptorPool getDescriptorPool(){return descriptorPool; }

        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void resetPool();

    private:
        Device &lveDevice;
        VkDescriptorPool descriptorPool;

        friend class LveDescriptorWriter;
    };

    class LveDescriptorWriter {
    public:
        LveDescriptorWriter(LveDescriptorSetLayout &setLayout, LveDescriptorPool &pool);

        LveDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        LveDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        LveDescriptorSetLayout &setLayout;
        LveDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace lve