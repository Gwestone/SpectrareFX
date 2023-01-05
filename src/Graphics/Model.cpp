#include <cstring>
#include "Model.h"

Model::Model(Device &_device, const Builder &_builder) : device(_device) {

    createVertexBuffers(_builder.vertices);
    createIndexBuffers(_builder.indices);
}

Model::~Model() {
    vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
    vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);
    if (hasIndices){
        vkDestroyBuffer(device.getDevice(), indicesBuffer, nullptr);
        vkFreeMemory(device.getDevice(), indicesBufferMemory, nullptr);
    }
}

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};

    bindingDescription.binding = 0;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindingDescription.stride = sizeof(Vertex);

    return bindingDescription;
}

std::vector <VkVertexInputAttributeDescription> Vertex::getAttributeDescription() {

    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptionsList(2);

    vertexInputAttributeDescriptionsList[0].binding = 0;
    vertexInputAttributeDescriptionsList[0].location = 0;
    //allocate 32 bits for every number in position vector
    vertexInputAttributeDescriptionsList[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributeDescriptionsList[0].offset = offsetof(Vertex, position);

    vertexInputAttributeDescriptionsList[1].binding = 0;
    vertexInputAttributeDescriptionsList[1].location = 1;
    //allocate 32 bits for every number in color vector
    vertexInputAttributeDescriptionsList[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributeDescriptionsList[1].offset = offsetof(Vertex, color);

    return vertexInputAttributeDescriptionsList;
}

void Model::createVertexBuffers(const std::vector<Vertex> &_vertexList) {
    vertexCount = static_cast<uint32_t>(_vertexList.size());
    VkDeviceSize allocSize = sizeof(_vertexList[0]) * vertexCount;

    device.createBuffer(allocSize,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        stagingVertexBuffer,
                        stagingVertexBufferMemory);

    void *data;
    vkMapMemory(device.getDevice(), stagingVertexBufferMemory, 0, allocSize, 0, &data);
    memcpy(data, _vertexList.data(), static_cast<size_t>(allocSize));
    vkUnmapMemory(device.getDevice(), stagingVertexBufferMemory);

    device.createBuffer(allocSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        vertexBuffer,
                        vertexBufferMemory);

    if (device.copyBuffer(stagingVertexBuffer, vertexBuffer, allocSize) != VK_SUCCESS){
        throw std::runtime_error("cant copy local buffer to gpu");
    };
    vkDestroyBuffer(device.getDevice(), stagingVertexBuffer, nullptr);
    vkFreeMemory(device.getDevice(), stagingVertexBufferMemory, nullptr);
}

void Model::createIndexBuffers(const std::vector<uint32_t> &_indicesList) {
    indicesCount = _indicesList.size();

    if (indicesCount > 0)
        hasIndices = true;

    if(!hasIndices)
        return;

    VkDeviceSize allocSize = sizeof(_indicesList[0]) * indicesCount;

    device.createBuffer(allocSize,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        stagingIndicesBuffer,
                        stagingIndicesBufferMemory);

    void *data;
    vkMapMemory(device.getDevice(), stagingIndicesBufferMemory, 0, allocSize, 0, &data);
    memcpy(data, _indicesList.data(), static_cast<size_t>(allocSize));
    vkUnmapMemory(device.getDevice(), stagingIndicesBufferMemory);

    device.createBuffer(allocSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        indicesBuffer,
                        indicesBufferMemory);

    if (device.copyBuffer(stagingIndicesBuffer, indicesBuffer, allocSize) != VK_SUCCESS){
        throw std::runtime_error("cant copy local buffer to gpu");
    };
    vkDestroyBuffer(device.getDevice(), stagingIndicesBuffer, nullptr);
    vkFreeMemory(device.getDevice(), stagingIndicesBufferMemory, nullptr);

}

void Model::bindDataToBuffer(const VkCommandBuffer &commandBuffer) {

    VkBuffer buffer[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffer, offsets);
    if (hasIndices)
        vkCmdBindIndexBuffer(commandBuffer, indicesBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Model::drawDataToBuffer(const VkCommandBuffer &commandBuffer) const {
    if (hasIndices)
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesCount), 1, 0, 0, 0);
    else
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

