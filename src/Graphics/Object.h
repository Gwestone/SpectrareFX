#ifndef SPECTRAREFX_OBJECT_H
#define SPECTRAREFX_OBJECT_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ZERO
#include <glm/vec2.hpp>
#include <glm/ext/matrix_float2x2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "Model.h"

struct TransformationPrimitive{
    glm::vec3 translation{};
    glm::vec3 scaleVector = {1.0f, 1.0f, 1.0f};
    glm::vec3 rotation{0.0f};

    glm::mat4 getTransformationMatrix();
    glm::mat4 getTransformationMatrixFAST();

};

class Object {
public:
    Object() {}

    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;
    Object(Object &&) = default;
    Object &operator=(Object &&) = default;

public:
    std::unique_ptr<Model> mesh;
    TransformationPrimitive transform;
};

#endif //SPECTRAREFX_OBJECT_H
