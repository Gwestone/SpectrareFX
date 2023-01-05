#include <iostream>
#include "Window.h"

void Window::init_window() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(window, this);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    setCallbacks();
}

void Window::clearWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

Window::Window(int _height, int _width, const std::string& _windowTitle) : height(_height), width(_width), windowTitle(_windowTitle) {
    init_window();
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

GLFWwindow* Window::getWindowObj() {
    return window;
}

Window::~Window() {
    clearWindow();
}

void Window::setCallbacks() {
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
}

void Window::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {

    Window* windowObj = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        std::cout << "trigger press" << std::endl;
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        windowObj->cursorPos.x = static_cast<int>(x);
        windowObj->cursorPos.y = static_cast<int>(y);

        windowObj->RIGHT_MOUSE_PRESSED = true;

        return;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
        std::cout << "trigger release" << std::endl;
        windowObj->cursorPos.x = 0;
        windowObj->cursorPos.y = 0;

        windowObj->RIGHT_MOUSE_PRESSED = false;

        return;
    }
}

void Window::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    Window* windowObj = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (windowObj->RIGHT_MOUSE_PRESSED){

        double xDiff = xpos - windowObj->cursorPos.x;
        double yDiff = ypos - windowObj->cursorPos.y;

        windowObj->deltaCursorPos = {static_cast<int>(xDiff), static_cast<int>(yDiff)};

        windowObj->cursorPos.x = static_cast<int>(xpos);
        windowObj->cursorPos.y = static_cast<int>(ypos);
        return;
    }
}

