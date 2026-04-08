#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "EventHandler.h"

namespace myGl {

class EventHandler;


enum class Direction {
    Right,
    Left,
    Front,
    Back
};

/**
 * Camera base class. Should not be used as a camera object. Rather use the specific subclass for specific task.
 */
class Camera {
public:
    Camera(float fov, int width, int height);
    Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float fov, int width, int height);
    Camera(const Camera& other) = delete;
    Camera& operator=(const Camera& other) = delete;
    virtual ~Camera();
    virtual void move(const float& speed, Direction dir);

    [[nodiscard]] glm::mat4 view() const {return glm::lookAt(position, position + front, up);};
    [[nodiscard]] glm::vec3 getPos() const {return this->position;};
    [[nodiscard]] glm::vec3 getFront() const {return this->front;};

    //all functions for mouse callbacks
    void setEventHandler(myGl::EventHandler* eventHandler){this->eventHandler = eventHandler;};
    void changeViewDir(double xPos, double yPos);
    void zoom(double yOffset);

protected:
    glm::vec3 position = glm::vec3{0.f, 0.f, 5.f};
    glm::vec3 front = glm::vec3{0.0f, 0.0f, -1.f};
    glm::vec3 up = glm::vec3{0.f, 1.f, 0.f};
    float fov;
    float aspect_ratio;

    //variables for camera angle and mouse callbacks
    float lastX;
    float lastY;
    float yaw = -90.f;
    float pitch = 0.f;
    bool first_mouse = true;

    myGl::EventHandler* eventHandler = nullptr;
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
