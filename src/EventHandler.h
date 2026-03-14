#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "Window.h"
#include "Camera.h"

namespace myGl {

/** Eventhandler that bundles all callback functions for glfw.
 *  Is a singleton to ensure correct handling of callbacks.
 */
class EventHandler {

    //private default constructor to ensure singleton properties
    EventHandler(){};

    //window events
    myGl::Window* window = nullptr;
    void framebufferCallback(int32_t width, int32_t height);

    //camera events
    myGl::Camera* camera = nullptr;
    void cursorCallback(double xPos, double yPos);
    void scrollCallback(double xOffset, double yOffset);

public:
    //create as Singleton
    static EventHandler& getInstance(myGl::Window* window) {
        static EventHandler eventHandler;
        //set WindowCallbacks to immediately set WindowUserPointer = this
        eventHandler.setWindowCallbacks(window);
        return eventHandler;
    }
    ~EventHandler();

    EventHandler(const EventHandler& other) = delete;
    EventHandler& operator=(const EventHandler& other) = delete;
    EventHandler(EventHandler&& other) = delete;
    EventHandler& operator=(EventHandler&& other) = delete;


    //set and unset methods for callbacks
    void setWindowCallbacks(myGl::Window* window);
    void unsetWindowCallbacks();
    void setCameraCallbacks(myGl::Camera* camera);




};






} //namespace myGl


#endif //EVENTHANDLER_H
