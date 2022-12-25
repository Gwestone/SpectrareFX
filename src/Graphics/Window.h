#ifndef PARALLEL_WINDOW_H
#define PARALLEL_WINDOW_H

#include <GLFW/glfw3.h>
#include <string>

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
};

#endif //PARALLEL_WINDOW_H
