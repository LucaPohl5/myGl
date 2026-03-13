#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"

namespace myGl {

float sensitivity = 0.1;

//sets all cameras sensitivity to sens
void setSensitivity(const float& sens) {
    sensitivity = sens;
}

Camera::Camera(float fov, int width, int height) :
    fov{fov}, aspect_ratio{static_cast<float>(width) / static_cast<float>(height)},
    lastX{static_cast<float>(width) / 2.f}, lastY{static_cast<float>(height) / 2.f} {}


Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float fov, int width, int height) :
    position{pos}, front{front}, up{up},
    fov{fov}, lastX{static_cast<float>(width) / 2}, lastY{static_cast<float>(height) / 2},
    aspect_ratio{static_cast<float>(width) / static_cast<float>(height)} {}


/**
 * sets the glfw mouseCursorPosCallback and mouseScrollCallback to change this cameras view
 */
void Camera::setCallbacks(GLFWwindow* window) {
    is_active = true;
    active_window = window;
    glfwSetWindowUserPointer(window, this);
    auto cursor_pos_callback = [](GLFWwindow* window, double xpos, double ypos) {
        static_cast<Camera*>(glfwGetWindowUserPointer(window))->moveCursorCallback(window, xpos, ypos);
    };
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    auto lambda_scroll_callback = [](GLFWwindow* window, double xoffset, double yoffset) {
        static_cast<Camera*>(glfwGetWindowUserPointer(window))->scrollCallback(window, xoffset, yoffset);
    };
    glfwSetScrollCallback(window, lambda_scroll_callback);
};

/**
 * unsets all the window callbacks for this camera
 */
void Camera::unsetCallbacks() {
    if (is_active) {
        glfwSetWindowUserPointer(active_window, nullptr);
        glfwSetCursorPosCallback(active_window, nullptr);
        glfwSetScrollCallback(active_window, nullptr);
        is_active = false;
    }
}

/**
 * callback function for when the mouse is moved.
 * The active camera (camera with bound callbacks) front vector is moved by the amount the mouse is moved.
 * Uses the sensitivity set by setSensitivity(float) to scale the movement
 * @param window window for which the callback functions are set
 * @param xPos x position of the mouse
 * @param yPos y position of the mouse
 */
void Camera::moveCursorCallback(GLFWwindow* window, double xPos, double yPos) {
    auto xpos = static_cast<float>(xPos);
    auto ypos = static_cast<float>(yPos);
    if (first_mouse) {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= -sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.f) {
        pitch = 89.f;
    }
    if (pitch < -89.f) {
        pitch = -89.f;
    }

    glm::vec3 direction;
    direction.x = std::cos(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch));
    direction.y = std::sin(glm::radians(this->pitch));
    direction.z = std::sin(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch));
    front = glm::normalize(direction);
}

/**
 * callback function for when the mouse scrollwheel is used. Changes the zoom of the camera by changing its fov
 * @param window window for which the callback function is set
 * @param xOffset not used
 * @param yOffset value by which the fov of the camera is downsized
 */
void Camera::scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    fov -= static_cast<float>(yOffset);
    if (fov < 1.f) {
        fov = 1.f;
    }
    if (fov > 45.f) {
        fov = 45.f;
    }
}

FlyingCamera::FlyingCamera(int32_t width, int32_t height, float fov, float zNear, float zFar) :
    Camera{fov, width, height},
    zNear{zNear}, zFar{zFar} {}


FlyingCamera::FlyingCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, int32_t width, int32_t height, float fov, float zNear, float zFar) :
    Camera{pos, front, up, fov, width, height},
    zNear{zNear}, zFar{zFar} {}

glm::mat4 FlyingCamera::perspective() {
    return glm::perspective(glm::radians(fov), aspect_ratio, zNear, zFar);
}

/**
 * moves the camera in a certain direction
 * @param speed the speed which the camera moves with (should be dependant on time)
 * @param dir the direction in which to move (can be Front, Back, Left or Right)
 */
void FlyingCamera::move(const float& speed, Direction dir) {
    if (dir == Direction::Front) {
        position += speed * front;
        return;
    }
    if (dir == Direction::Back) {
        position -= speed * front;
        return;
    }
    const auto cameraRight = glm::normalize(glm::cross(front, up));
    if (dir == Direction::Right) {
        position += speed * cameraRight;
        return;
    }
    if (dir == Direction::Left) {
        position -= speed * cameraRight;
    }
}









}
