#include <cstring>
#include "Model.h"

Model::Model(Device &_device, const std::vector<Vertex> &_vertices) : device(_device) {

    createVertexBuffers(_vertices);
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
    VkDeviceSize allocSize = sizeof(_vertexList[0]) * _vertexList.size();

    /*
     device.createBuffer(allocSize,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            //VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                        0,
                        vertexBuffer,
                        vertexBufferMemory);
     */
    device.createBuffer(allocSize,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        stagingBuffer,
                        stagingBufferMemory);

    void *data;
    vkMapMemory(device.getDevice(), stagingBufferMemory, 0, allocSize, 0, &data);
    memcpy(data, _vertexList.data(), static_cast<size_t>(allocSize));
    vkUnmapMemory(device.getDevice(), stagingBufferMemory);

    device.createBuffer(allocSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        vertexBuffer,
                        vertexBufferMemory);

    if (device.copyBuffer(stagingBuffer, vertexBuffer, allocSize) != VK_SUCCESS){
        throw std::runtime_error("cant copy local buffer to gpu");
    };
    vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
}

void Model::bindDataToBuffer(const VkCommandBuffer &commandBuffer) {

    VkBuffer buffer[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffer, offsets);
}

void Model::drawDataToBuffer(const VkCommandBuffer &commandBuffer) const {
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

glm::mat2 Model::rotateTransformationMatrix(const glm::mat2 &_transformation, float rad) {
    const float c = glm::cos(rad);
    const float s = glm::sin(rad);

    glm::mat2 rotation = {{c, -s},
                          {s, c}};

    return _transformation * rotation;
}

