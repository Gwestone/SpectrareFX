#ifndef PARALLEL_WINDOW_H
#define PARALLEL_WINDOW_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ZERO
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/common.hpp>

#include <GLFW/glfw3.h>
#include <string>
#include <vulkan/vulkan_core.h>

struct CursorPos{
    int x = 0;
    int y = 0;
};

struct Rotations{
private:
    float x = 0.0f;
    float y = 0.0f;
public:
    void addX(float _x){
        x = glm::mod((x + _x), glm::two_pi<float>());
    }
    void addY(float _y){
        y = glm::mod((y + _y), glm::two_pi<float>());
    }
    float getX() const{return x;}
    float getY() const{return y;}
};


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

    Rotations& getRotation(){return rotate;};

    bool RIGHT_MOUSE_PRESSED = false;

private:
    void setCallbacks();
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    CursorPos cursorPos{};
    Rotations rotate{};
};

#endif //PARALLEL_WINDOW_H
