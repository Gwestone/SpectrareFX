#include "Object.h"

glm::mat4 TransformationPrimitive::getTransformationMatrix() {

    auto transform = glm::translate(glm::mat4(1.0f), translation);
    transform = glm::scale(transform, scaleVector);
    transform = glm::rotate(transform, rotation.x, {1.0f, 0.0f, 0.0f});
    transform = glm::rotate(transform, rotation.y, {0.0f, 1.0f, 0.0f});
    transform = glm::rotate(transform, rotation.z, {0.0f, 0.0f, 1.0f});

    return transform;
}

glm::mat4 TransformationPrimitive::getTransformationMatrixFAST() {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    return glm::mat4{
            {
                    scaleVector.x * (c1 * c3 + s1 * s2 * s3),
                    scaleVector.x * (c2 * s3),
                    scaleVector.x * (c1 * s2 * s3 - c3 * s1),
                                                          0.0f,
            },
            {
                    scaleVector.y * (c3 * s1 * s2 - c1 * s3),
                    scaleVector.y * (c2 * c3),
                    scaleVector.y * (c1 * c3 * s2 + s1 * s3),
                                                          0.0f,
            },
            {
                    scaleVector.z * (c2 * s1),
                    scaleVector.z * (-s2),
                    scaleVector.z * (c1 * c2),
                                                          0.0f,
            },
            {translation.x, translation.y, translation.z, 1.0f}};
}
