#include <iostream>
#include <cstdlib>
#include "Window.h"

namespace myGl {
/**
 * initiate glfw (replacement for glfw initiation functions)
 */
void init() {
    if (glfwInit() == GLFW_FALSE) {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        std::exit(EXIT_FAILURE);
    };
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


}

/**
 * terminate glfw (replacement for glfw functions)
 */
void terminate() {
    glfwTerminate();
}

Window::Window(int32_t width, int32_t height, const std::string& title) :
width{width}, height{height}, canvasRegions{}{
    window_ptr = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window_ptr == nullptr) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window_ptr);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    int32_t fbwidth, fbheight;
    glfwGetFramebufferSize(window_ptr, &fbwidth, &fbheight);
    this->width = fbwidth;
    this->height = fbheight;
}


/**
 * when destroying a window, set all canvases to have no parent window and unset Window Callbacks
 */
Window::~Window() {
    for (auto canvas : canvasRegions) {
        canvas->parent_window = nullptr;
    }
    this->eventHandler->unsetWindowCallbacks();
}



void Window::resize(const int32_t width, const int32_t height) {
    this->width = width;
    this->height = height;

    //move and scale canvases to match window changes
    for (auto canvas : canvasRegions) {
        canvas->width = static_cast<int32_t>(std::round(static_cast<float>(width) * canvas->widthRatio));
        canvas->height = static_cast<int32_t>(std::round(static_cast<float>(height) * canvas->heightRatio));
        canvas->xOffset = static_cast<int32_t>(std::round(static_cast<float>(width) * canvas->xRatio));
        canvas->yOffset = static_cast<int32_t>(std::round(static_cast<float>(height) * canvas->yRatio));
    }
}

void Window::addCanvas(myGl::Canvas* canvas) {
    this->canvasRegions.push_back(canvas);
}

//getter methods
GLFWwindow* Window::getWindow() {
    return this->window_ptr;
}

myGl::Canvas createFullWindowCanvas(myGl::Window& window) {
    int32_t fbwidth, fbheight;
    glfwGetFramebufferSize(window.getWindow(), &fbwidth, &fbheight);
    return Canvas{fbwidth, fbheight, 0, 0, &window};
}



}