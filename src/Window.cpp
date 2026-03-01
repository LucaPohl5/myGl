#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include "Window.h"

namespace myGl {
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

    Window::Window(int32_t width, int32_t height, const std::string& title) : width{width}, height{height} {
        this->window_ptr = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (this->window_ptr == nullptr) {
            std::cout << "Failed to create GLFW window." << std::endl;
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(this->window_ptr);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD." << std::endl;
            std::exit(EXIT_FAILURE);
        }

        int32_t fbwidth, fbheight;
        glfwGetFramebufferSize(this->window_ptr, &fbwidth, &fbheight);
        glViewport(0, 0, fbwidth, fbheight);

        glfwSetWindowUserPointer(window_ptr, this);
        auto framebuffer_callback = [](GLFWwindow* window, int32_t width, int32_t height) {
            static_cast<Window*>(glfwGetWindowUserPointer(window))->resize(width, height);
        };
        glfwSetFramebufferSizeCallback(window_ptr, framebuffer_callback);
    }

    void Window::resize(const int32_t width, const int32_t height) {
        this->width = width;
        this->height = height;
        glViewport(0, 0, width, height);
    }

    //getter methods
    GLFWwindow* Window::getWindow() {
        return this->window_ptr;
    }
}