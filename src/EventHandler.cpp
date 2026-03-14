#include "EventHandler.h"
#include <iostream>


namespace myGl {

EventHandler::~EventHandler() {
    if (this->window != nullptr){this->window->setEventHandler(nullptr);};
    if (this->camera != nullptr){this->camera->setEventHandler(nullptr);};
}



/**
 * set FramebufferSizeCallback for this window
 * @param window window for which to set callbacks (gets made to Context)
 */
void EventHandler::setWindowCallbacks(myGl::Window* window) {
    this->window = window;
    window->setEventHandler(this);
    auto window_ptr = window->getWindow();
    glfwMakeContextCurrent(window_ptr);

    glfwSetWindowUserPointer(window_ptr, this);

    auto framebuffer_callback = [](GLFWwindow* window, int32_t width, int32_t height) {
        static_cast<EventHandler*>(glfwGetWindowUserPointer(window))->framebufferCallback(width, height);
    };
    glfwSetFramebufferSizeCallback(window_ptr, framebuffer_callback);
}

/**
 * Function to unset the callbacks for a window. Gets called when this->window is deleted.
 */
void EventHandler::unsetWindowCallbacks() {
    if (this->window == nullptr){return;}
    glfwSetFramebufferSizeCallback(this->window->getWindow(), nullptr);
    this->window = nullptr;
}

void EventHandler::framebufferCallback(int32_t width, int32_t height) {
    assert(this->window != nullptr);
    this->window->resize(width, height);
}





//TODO: continue here!!! implement logic to for camera deletion (that camera pointer here = null) and implement camera callback functions + testing of eventHandler
void EventHandler::setCameraCallbacks(myGl::Camera* camera) {
    if (this->window == nullptr) {
        std::cerr << "EventHandler currently has no window connected! Did you forget to connect a window first?" << std::endl;
        return;
    }
    if (this->camera != nullptr) { //if camera is already bound, unset the old camera
        this->camera->setEventHandler(nullptr);
    }

    auto window_ptr = this->window->getWindow();
    glfwSetWindowUserPointer(window_ptr, this);

    this->camera = camera;
    this->camera->setEventHandler(this);
    auto cursor_pos_callback = [](GLFWwindow* window, double xpos, double ypos) {
        static_cast<EventHandler*>(glfwGetWindowUserPointer(window))->cursorCallback(xpos, ypos);
    };
    glfwSetCursorPosCallback(window_ptr, cursor_pos_callback);

    auto lambda_scroll_callback = [](GLFWwindow* window, double xoffset, double yoffset) {
        static_cast<EventHandler*>(glfwGetWindowUserPointer(window))->scrollCallback(xoffset, yoffset);
    };
    glfwSetScrollCallback(window_ptr, lambda_scroll_callback);
}


void EventHandler::unsetCameraCallbacks() {
    if (this->window == nullptr) {
        std::cerr << "Trying to unbind camera, with no window connected! Unbind the window from the event handler last!" << std::endl;
        exit(EXIT_FAILURE);
    }
    glfwSetCursorPosCallback(this->window->getWindow(), nullptr);
    glfwSetScrollCallback(this->window->getWindow(), nullptr);
    this->camera = nullptr;
}

void EventHandler::cursorCallback(double xpos, double ypos) {
    assert(this->camera != nullptr);
    this->camera->changeViewDir(xpos, ypos);
}

void EventHandler::scrollCallback(double xOffset, double yOffset) {
    assert(this->camera != nullptr);
    this->camera->zoom(yOffset);
}



} //namespace myGl
