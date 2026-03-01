#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace myGl {

enum class Direction {
    Right,
    Left,
    Front,
    Back
};

//Camera base class
class Camera {
public:
    Camera(float fov, float aspect_ratio);
    Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float fov, float aspect_ratio);
    virtual ~Camera(){ this->unsetCallbacks();};
    virtual void move(const float& speed, Direction dir){};

    glm::mat4 view() const {return glm::lookAt(position, position + front, up);};
    glm::vec3 getPos() const {return this->position;};
    glm::vec3 getFront() const {return this->front;};

    //all functions for mouse callbacks
    void setCallbacks(GLFWwindow* window);
    void unsetCallbacks();
    void moveCursorCallback(GLFWwindow* window, double xPos, double yPos);
    void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
protected:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float fov;
    float aspect_ratio;

    //variables for camera angle and mouse callbacks
    float lastX = 0.f;
    float lastY = 0.f;
    float yaw = -90.f;
    float pitch = 0.f;
    bool first_mouse = true;
    bool is_active = false;
    GLFWwindow* active_window = nullptr;
};

/**
 * a free flying camera that inherits from Camera
 * - can freely move in all directions
 * - has perspective transformation
 */
class FlyingCamera : public Camera {
    float zNear;
    float zFar;
public:
    FlyingCamera(int32_t width, int32_t height, float fov, float zNear, float zFar);
    FlyingCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, int32_t width, int32_t height, float fov, float zNear, float zFar);
    FlyingCamera(const FlyingCamera& other) = delete;
    FlyingCamera& operator=(const FlyingCamera& other) = delete;
    ~FlyingCamera() override = default;
    void move(const float& speed, Direction dir) override;
    glm::mat4 perspective();
};

void setSensitivity(const float& sens);

}


#endif
