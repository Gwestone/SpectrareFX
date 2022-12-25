#include "Window.h"

void Window::init_window() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
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

