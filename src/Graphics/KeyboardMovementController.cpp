#include "KeyboardMovementController.h"

void KeyboardMovementController::moveInPlaneXZ(Window &window, float dt, Object &_cameraObject) {

    glm::vec3 rotate{0};
    if (glfwGetKey(window.getWindowObj(), KeyMappings::LOOK_RIGHT) == GLFW_PRESS) rotate.y += 1.f;
    if (glfwGetKey(window.getWindowObj(), KeyMappings::LOOK_LEFT) == GLFW_PRESS) rotate.y -= 1.f;
    if (glfwGetKey(window.getWindowObj(), KeyMappings::LOOK_UP) == GLFW_PRESS) rotate.x += 1.f;
    if (glfwGetKey(window.getWindowObj(), KeyMappings::LOOK_DOWN) == GLFW_PRESS) rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        _cameraObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
    }

    // limit pitch values between about +/- 85ish degrees
    _cameraObject.transform.rotation.x = glm::clamp(_cameraObject.transform.rotation.x, -1.5f, 1.5f);
    _cameraObject.transform.rotation.y = glm::mod(_cameraObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = _cameraObject.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 moveDir{0.f};
    if (glfwGetKey(window.getWindowObj(), KeyMappings::MOVE_FORWARD) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window.getWindowObj(), KeyMappings::MOVE_BACKWARD) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window.getWindowObj(), KeyMappings::MOVE_RIGHT) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window.getWindowObj(), KeyMappings::MOVE_LEFT) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window.getWindowObj(), KeyMappings::MOVE_UP) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window.getWindowObj(), KeyMappings::MOVE_DOWN) == GLFW_PRESS) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        _cameraObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
    }

}
