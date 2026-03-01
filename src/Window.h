
#ifndef WINDOW_H
#define WINDOW_H

#include <cstdint>
#include <GLFW/glfw3.h>

namespace myGl {

//initiates glfw and sets window hints
void init();


class Window {
public:
    Window(int32_t width, int32_t height, const std::string& title);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    void resize(int32_t width, int32_t height);
    GLFWwindow* getWindow();

private:
    int32_t width;
    int32_t height;
    GLFWwindow* window_ptr;
};

}

#endif
