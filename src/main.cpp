#include <cstdlib>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Camera.h"

int main(){

    myGl::init();
    myGl::Window w1{800, 600, "Test"};
    myGl::FlyingCamera camera1{800, 600, 45, 1.f, 100.f};
    camera1.setCallbacks(w1.getWindow());
    myGl::setSensitivity(1.f);
    myGl::FlyingCamera camera2{800, 600, 45, 1.f, 100.f};

    while (!glfwWindowShouldClose(w1.getWindow())) {
        glClearColor(1.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);


        if (glfwGetKey(w1.getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            camera2.setCallbacks(w1.getWindow());
        }


        glfwSwapBuffers(w1.getWindow());
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}