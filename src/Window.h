
#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Canvas.h"
#include "EventHandler.h"

namespace myGl {

//initiates glfw and sets window hints
void init();
void terminate();



class Window {
public:
    Window(int32_t width, int32_t height, const std::string& title);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    ~Window();
    void resize(int32_t width, int32_t height);
    void addCanvas(myGl::Canvas* canvas);
    [[nodiscard]] bool shouldClose() const {return glfwWindowShouldClose(window_ptr);};


    GLFWwindow* getWindow();
    void setEventHandler(myGl::EventHandler* eventHandler){this->eventHandler = eventHandler;}

    friend class Canvas;

    /**user defined implicit conversion to GLFWwindow* to let glfw functions be run by passing Window class for testing purposes*/
    operator GLFWwindow*() const {return this->window_ptr;}
#warning "TODO: remove this implicit conversion later (only for easier testing purposes now)"

private:
    int32_t width; //CORRESPONDS TO THE FRAMEBUFFER SIZE NOT THE WINDOW SIZE
    int32_t height;//CORRESPONDS TO THE FRAMEBUFFER SIZE NOT THE WINDOW SIZE
    GLFWwindow* window_ptr;
    std::vector<myGl::Canvas*> canvasRegions;
    myGl::EventHandler* eventHandler = nullptr;
};



[[nodiscard]] myGl::Canvas createFullWindowCanvas(myGl::Window& window);

}

#endif
