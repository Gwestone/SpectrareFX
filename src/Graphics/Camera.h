#ifndef SPECTRAREFX_CAMERA_H
#define SPECTRAREFX_CAMERA_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ZERO
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>

class Camera {
public:
    Camera(){};
    void setOrthographicProjection( float left, float right, float top, float bottom, float near, float far);
    void setProspectiveProjection(float fov, float aspect, float near, float far);
    glm::mat4& getProjectionMatrix();

    void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = {0.0f, -1.0f, 0.0f});
    void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = {0.0f, -1.0f, 0.0f});
    void setViewXYZ(glm::vec3 position, glm::vec3 rotation);

    glm::mat4& getViewMatrix();
private:
    glm::mat4 projectionMatrix{1.0f};
    glm::mat4 viewMatrix{1.0f};
};

#endif //SPECTRAREFX_CAMERA_H
