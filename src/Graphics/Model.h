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

struct Builder{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

class Model {
private:
    Device& device;

    //vertex buffer
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    //-----------------------------------
    VkBuffer stagingVertexBuffer;
    VkDeviceMemory stagingVertexBufferMemory;

    //indices buffer
    VkBuffer indicesBuffer;
    VkDeviceMemory indicesBufferMemory;
    //-----------------------------------
    VkBuffer stagingIndicesBuffer;
    VkDeviceMemory stagingIndicesBufferMemory;

    uint32_t vertexCount = 0;

    uint32_t indicesCount = 0;
    bool hasIndices = false;
public:
    Model(Device &_device, const Builder &_builder);
    ~Model();

    void createVertexBuffers(const std::vector<Vertex>& _vertexList);

    void createIndexBuffers(const std::vector<uint32_t>& _indicesList);

    void bindDataToBuffer(const VkCommandBuffer &commandBuffer);
    void drawDataToBuffer(const VkCommandBuffer &commandBuffer) const;

};


#endif //SPECTRAREFX_MODEL_H
