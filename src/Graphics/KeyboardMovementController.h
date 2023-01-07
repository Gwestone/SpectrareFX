#pragma once

#include "Camera.h"

class KeyboardMovementController {
public:
    enum KeyMappings {
        MOVE_LEFT = GLFW_KEY_A,
        MOVE_RIGHT = GLFW_KEY_D,
        MOVE_FORWARD = GLFW_KEY_W,
        MOVE_BACKWARD = GLFW_KEY_S,
        MOVE_UP = GLFW_KEY_E,
        MOVE_DOWN = GLFW_KEY_Q,
        LOOK_LEFT = GLFW_KEY_LEFT,
        LOOK_RIGHT = GLFW_KEY_RIGHT,
        LOOK_UP = GLFW_KEY_UP,
        LOOK_DOWN = GLFW_KEY_DOWN
    };
public:

    void moveInPlaneXZ(Window &window, float dt, Object &_cameraObject);

public:

    KeyMappings keys{};
    float moveSpeed = 1.0f;
    float lookSpeed = 1.0f;
};