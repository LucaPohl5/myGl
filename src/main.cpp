#include <cstdlib>
#include <iostream>

#include "myGl.h"



void process_input(GLFWwindow* window, myGl::Camera* camera, float dtime);

int main(){
    float oldTime = glfwGetTime();
    float dtime;


    myGl::init();
    myGl::Window w1{800, 600, "Test"};
    myGl::EventHandler& eventHandler = myGl::EventHandler::getInstance(&w1);


    myGl::Canvas canvas = myGl::createFullWindowCanvas(w1);

    myGl::Canvas miniCanvas{100, 100, 300, 300, &w1};


    float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    // Right face
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left
    };

    uint32_t VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);




    myGl::FlyingCamera camera1{800, 600, 45, 1.1f, 100.f};
    //camera1.setCallbacks(w1);
    //glfwSetInputMode(w1, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //create shader
    myGl::Shader basic_shader("basic.v.glsl", "basic.f.glsl");
    basic_shader.use();
    basic_shader.setMat4("model", glm::mat4{1.f});


    while (!w1.shouldClose()) {
        float currentTime = glfwGetTime();
        dtime = currentTime - oldTime;
        oldTime = currentTime;
        process_input(w1, &camera1, dtime);


        canvas.clear(0.f, 1.f, 0.f, 1.f);
        miniCanvas.clear(1.f, 0.f, 1.f, 1.f);

        glBindVertexArray(VAO);
        basic_shader.use();
        basic_shader.setMat4("view", camera1.view());
        basic_shader.setMat4("projection", camera1.perspective());
        glDrawArrays(GL_TRIANGLES, 0, 36);



        glfwSwapBuffers(w1.getWindow());
        glfwPollEvents();
    }
    myGl::terminate();
    return EXIT_SUCCESS;
}




void process_input(GLFWwindow* window, myGl::Camera* camera, float dtime) {
    const float camera_speed = 2.5f * dtime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->move(camera_speed, myGl::Direction::Front);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->move(camera_speed, myGl::Direction::Back);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->move(camera_speed, myGl::Direction::Left);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->move(camera_speed, myGl::Direction::Right);
    }
}
