#include <iostream>
#include <glad/glad.h>
#include <cstdlib>
#include <stb/stb_image.h>
#include "Window.h"
#include "EventHandler.h"

namespace myGl {
/**
 * initiate glfw (replacement for glfw initiation functions) +
 * set stb flags
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
    stbi_set_flip_vertically_on_load(true);
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


Canvas::Canvas(int width, int height, int x, int y, myGl::Window* window) :
    width{width}, height{height}, xOffset{x}, yOffset{y}, parent_window{window} {
    window->addCanvas(this);
    widthRatio = static_cast<float>(width) / static_cast<float>(window->width);
    heightRatio = static_cast<float>(height)/ static_cast<float>(window->height);
    xRatio = static_cast<float>(x) / static_cast<float>(window->width);
    yRatio = static_cast<float>(y) / static_cast<float>(window->height);
    xRatio = xRatio > 1 ? 1 : xRatio;
    yRatio = yRatio > 1 ? 1 : yRatio;
    widthRatio = widthRatio > 1 ? 1 : widthRatio;
    heightRatio = heightRatio > 1 ? 1 : heightRatio;
}


/**
 * when deleting a canvas, also remove it from the windows canvasRegions
 */
Canvas::~Canvas() {
    parent_window->canvasRegions.erase(find(parent_window->canvasRegions.begin(), parent_window->canvasRegions.end(), this));
}

/**
 * resize the canvas to the new width and height
 * TODO: DO NOT USE FOR SCALING WITH WINDOW
 */
void Canvas::resize(int width, int height) {
    this->width = width;
    this->height = height;
    widthRatio = static_cast<float>(width) / static_cast<float>(parent_window->width);
    heightRatio = static_cast<float>(height) / static_cast<float>(parent_window->height);
}

/**
 * move the viewport of this canvas
 * TODO: DO NOT USE FOR SCALING WITH WINDOW
 * @param newX new x coordinate of viewport
 * @param newY new y coordinate of viewport
 */
void Canvas::move(int newX, int newY) {
    this->xOffset = newX;
    this->yOffset = newY;
    xRatio = static_cast<float>(newX) / static_cast<float>(parent_window->width);
    yRatio = static_cast<float>(newY) / static_cast<float>(parent_window->height);
}

/**
 * clear the viewport corresponding to this canvas.
 * If depth testing is enabled, also clears the z-Buffer
 */
void Canvas::clear(float red, float green, float blue, float alpha) {
    if (parent_window) { //if window to which is displayed still exists
        //get old viewport info and set viewport to this canvas
        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);
        glViewport(xOffset, yOffset, width, height);

        //clear canvas
        glScissor(xOffset, yOffset, width, height);
        glEnable(GL_SCISSOR_TEST);
        glClearColor(red, green, blue, alpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        //set old viewport back
        glViewport(vp[0], vp[1], vp[2], vp[3]);
    }else {
        std::cout << "Window for this canvas doesnt exist!" << std::endl;
    }
}

/**
 * tells opengl to draw to this canvas
 */
void Canvas::drawTo() {
    if (parent_window) { //if window to which is displayed still exists
        glViewport(xOffset, yOffset, width, height);
    }else {
        std::cout << "Window for this canvas doesnt exist!" << std::endl;
    }
}

}