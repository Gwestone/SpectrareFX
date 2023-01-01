#ifndef PARALLEL_WINDOW_H
#define PARALLEL_WINDOW_H

#include <GLFW/glfw3.h>
#include <string>
#include <vulkan/vulkan_core.h>

class Window {
private:
    int height;
    int width;
    std::string windowTitle;
    GLFWwindow* window;
public:
    Window(int _height, int _width, const std::string &_windowTitle);
    void init_window();
    void clearWindow();
    bool shouldClose();
    GLFWwindow* getWindowObj();
    ~Window();

    VkExtent2D getExtent() const {return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};};
};

#endif //PARALLEL_WINDOW_H
