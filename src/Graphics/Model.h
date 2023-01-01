#ifndef SPECTRAREFX_MODEL_H
#define SPECTRAREFX_MODEL_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ZERO
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <glm/ext/matrix_float3x3.hpp>
#include "Device.h"

struct Vertex{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 color = {1.0f, 0.0f, 0.0f};

    static VkVertexInputBindingDescription getBindingDescription();
    static std::vector <VkVertexInputAttributeDescription> getAttributeDescription();

};

class Model {
private:
    Device& device;
    VkBuffer vertexBuffer;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;
public:
    Model(Device &_device, const std::vector<Vertex> &_vertices);
    static glm::mat2 rotateTransformationMatrix(const glm::mat2 &_transformation, float rad);

    void createVertexBuffers(const std::vector<Vertex>& _vertexList);
    void bindDataToBuffer(const VkCommandBuffer &commandBuffer);
    void drawDataToBuffer(const VkCommandBuffer &commandBuffer) const;

};


#endif //SPECTRAREFX_MODEL_H
