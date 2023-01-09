#include "Model.h"

#include "imguiImports.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace std {
    template <>
    struct hash<Vertex> {
        size_t operator()(Vertex const &vertex) const {
            size_t seed = 0;
            hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}


Model::Model(Device &_device, const Builder &_builder) : device(_device) {

    createVertexBuffers(_builder.vertices);
    createIndexBuffers(_builder.indices);
    createTextureBuffers(_builder.image);
}


Model::~Model() {}


VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};

    bindingDescription.binding = 0;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindingDescription.stride = sizeof(Vertex);

    return bindingDescription;
}


std::vector <VkVertexInputAttributeDescription> Vertex::getAttributeDescription() {

    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptionsList(4);

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

    vertexInputAttributeDescriptionsList[2].binding = 0;
    vertexInputAttributeDescriptionsList[2].location = 2;
    //allocate 32 bits for every number in color vector
    vertexInputAttributeDescriptionsList[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributeDescriptionsList[2].offset = offsetof(Vertex, normal);

    vertexInputAttributeDescriptionsList[3].binding = 0;
    vertexInputAttributeDescriptionsList[3].location = 3;
    //allocate 24 bits for every number in texture coordinate
    vertexInputAttributeDescriptionsList[3].format = VK_FORMAT_R32G32_SFLOAT;
    vertexInputAttributeDescriptionsList[3].offset = offsetof(Vertex, uv);

    return vertexInputAttributeDescriptionsList;
}


void Model::createVertexBuffers(const std::vector<Vertex> &_vertexList) {
    vertexCount = static_cast<uint32_t>(_vertexList.size());
    assert(vertexCount > 3 && "cant be mesh with less than 3 verices");
    VkDeviceSize instanceSize = sizeof(_vertexList[0]);

    Buffer stagingBuffer{
        device,
        instanceSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)_vertexList.data());
    stagingBuffer.unmap();

    vertexBuffer = std::make_unique<Buffer>(device,
                                            instanceSize,
                                            vertexCount,
                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), vertexBuffer->getBufferSize()) != VK_SUCCESS) {
        throw std::runtime_error("cant copy local buffer to gpu");
    };
}


void Model::createIndexBuffers(const std::vector<uint32_t> &_indicesList) {
    indicesCount = _indicesList.size();

    if (indicesCount > 0)
        hasIndices = true;

    if(!hasIndices)
        return;

    VkDeviceSize instanceSize = sizeof(_indicesList[0]);

    Buffer stagingBuffer{
            device,
            instanceSize,
            indicesCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)_indicesList.data());
    stagingBuffer.unmap();

    indexBuffer = std::make_unique<Buffer>(device,
                                           instanceSize,
                                           indicesCount,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), indexBuffer->getBufferSize()) != VK_SUCCESS){
        throw std::runtime_error("cant copy local buffer to gpu");
    };
}


void Model::createTextureBuffers(const ImageBuilder &_image) {

    if (_image.pixels)
        hasTexture = true;

    if (!hasTexture)
        return;

    std::cout << "loading texture" << std::endl;

    VkDeviceSize instanceSize = _image.width * _image.height * STBI_rgb_alpha;  

    //writing staging buffer
    Buffer stagingBuffer{
            device,
            instanceSize,
            1,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)_image.pixels);
    stagingBuffer.unmap();
    stbi_image_free(_image.pixels);

    textureBuffer = std::make_unique<ImageBuffer>(device,
                                                  VK_IMAGE_TYPE_2D,
                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                  VK_FORMAT_R8G8B8A8_SRGB,
                                                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                                  _image.width,
                                                  _image.height);

    textureBuffer->transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    auto res = device.copyBufferToImage(stagingBuffer.getBuffer(), textureBuffer->getImage(), static_cast<uint32_t>(_image.width), static_cast<uint32_t>(_image.height));
    if (res != VK_SUCCESS){
        throw std::runtime_error("failed to copy buffer to image");
    }
    textureBuffer->transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}


void Model::bindDataToBuffer(const VkCommandBuffer &commandBuffer) {

    VkBuffer buffer[] = {vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffer, offsets);
    if (hasIndices)
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}


void Model::drawDataToBuffer(const VkCommandBuffer &commandBuffer) const {
    if (hasIndices)
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesCount), 1, 0, 0, 0);
    else
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}


std::unique_ptr<Model> Model::loadFromFile(Device &device, const std::string &_modelFilepath = "", const std::string &_textureFilepath = "") {
    Builder builder{};

    if (!_modelFilepath.empty())
        builder.loadFromModelFile(_modelFilepath);
    if (!_textureFilepath.empty())
        builder.loadTextureFile(_textureFilepath);

    auto model = std::make_unique<Model>(device, builder);
    return model;
}


void Builder::loadFromModelFile(const std::string &filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                };

                auto colorIndex = 3 * index.vertex_index + 2;
                if (colorIndex < attrib.colors.size()) {
                    vertex.color = {
                            attrib.colors[colorIndex - 2],
                            attrib.colors[colorIndex - 1],
                            attrib.colors[colorIndex - 0],
                    };
                } else {
                    vertex.color = {1.f, 1.f, 1.f};  // set default color
                }
            }

            if (index.normal_index >= 0) {
                vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                };
            }

            if (index.texcoord_index >= 0) {
                vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
                };
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

void Builder::loadTextureFile(const std::string &filepath) {

    image.pixels = stbi_load(filepath.c_str(), &image.width, &image.height, &image.channels_size, STBI_rgb_alpha);

    if (!image.pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
}
