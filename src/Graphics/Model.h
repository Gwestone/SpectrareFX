#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ZERO
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_float3x3.hpp>

#include <unordered_map>
#include <cstring>
#include <memory>
#include <vulkan/vulkan.h>
#include <vector>

#include "utils.h"
#include "Buffer.h"
#include "ImageBuffer.h"

struct Vertex{
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 color{1.0f, 0.0f, 0.0f};
    glm::vec3 normal{};
    glm::vec2 uv{};

    static VkVertexInputBindingDescription getBindingDescription();
    static std::vector <VkVertexInputAttributeDescription> getAttributeDescription();

    bool operator==(const Vertex &_other) const{
        return position == _other.position && color == _other.color && normal == _other.normal && uv == _other.uv;
    }
};

struct ImageBuilder{
    void* pixels = nullptr;
    int width = 0;
    int height = 0;
    int channels_size = 0;
    bool initialized = false;
};

struct Builder{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    ImageBuilder image{};

    void loadFromModelFile(const std::string &filepath);
    void loadTextureFile(const std::string &filepath);
};

class Model {
private:

    Device& device;

    //vertex buffer
    std::unique_ptr<Buffer> vertexBuffer;

    //indices buffer
    std::unique_ptr<Buffer> indexBuffer;

    //texture buffer
    std::unique_ptr<ImageBuffer> textureBuffer;

    uint32_t vertexCount = 0;

    uint32_t indicesCount = 0;
    bool hasIndices = false;
    bool hasTexture = false;
public:
    Model(Device &_device, const Builder &_builder);
    ~Model();

    void createVertexBuffers(const std::vector<Vertex>& _vertexList);
    void createIndexBuffers(const std::vector<uint32_t>& _indicesList);
    void createTextureBuffers(const ImageBuilder &_image);

    void bindDataToBuffer(const VkCommandBuffer &commandBuffer);
    void drawDataToBuffer(const VkCommandBuffer &commandBuffer) const;

public:
    static std::unique_ptr<Model> loadFromFile(Device &device, const std::string &_modelFilepath, const std::string &_textureFilepath);

};