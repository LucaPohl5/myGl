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
    fov{fov}, aspect_ratio{static_cast<float>(width) / static_cast<float>(height)},
    lastX{static_cast<float>(width) / 2}, lastY{static_cast<float>(height) / 2} {}

Camera::~Camera() {
    if (this->eventHandler != nullptr){this->eventHandler->unsetCameraCallbacks();}
}

/*
 * TODO: do not use this move function! Always use derived functions move function!
 */
void Camera::move([[maybe_unused]] const float& speed, [[maybe_unused]] Direction dir) {
    std::cerr << "Base Camera move function used! Use derived class with overridden move function!" << std::endl;
};


/**
 * changes what the camera looks. Used  for mouse cursor callback
 * @param xPos new xPos of mouse cursor
 * @param yPos new yPos of mouse cursor
 */
void Camera::changeViewDir(double xPos, double yPos) {
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
 * simulates zooming of camera. Is used for scroll Callback
 * @param yOffset zoom value
 */
void Camera::zoom(double yOffset) {
    fov -= static_cast<float>(yOffset);
    if (fov < 1.f) {
        fov = 1.f;
    }
    if (fov > 70.f) {
        fov = 70.f;
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
