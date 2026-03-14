
#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <vector>

namespace myGl {
    //forward declarations
    class EventHandler;
    class Canvas;

    //initiates glfw and sets window hints
    void init();
    void terminate();


    class Window {
    public:
        Window(int32_t width, int32_t height, const std::string& title);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        ~Window();
        void resize(int32_t width, int32_t height);
        void addCanvas(myGl::Canvas* canvas);
        [[nodiscard]] bool shouldClose() const {return glfwWindowShouldClose(window_ptr);};


        GLFWwindow* getWindow();
        void setEventHandler(myGl::EventHandler* eventHandler){this->eventHandler = eventHandler;}

        friend class Canvas;

        /**user defined implicit conversion to GLFWwindow* to let glfw functions be run by passing Window class for testing purposes*/
        operator GLFWwindow*() const {return this->window_ptr;}
#warning "TODO: remove this implicit conversion later (only for easier testing purposes now)"

    private:
        int32_t width; //CORRESPONDS TO THE FRAMEBUFFER SIZE NOT THE WINDOW SIZE
        int32_t height;//CORRESPONDS TO THE FRAMEBUFFER SIZE NOT THE WINDOW SIZE
        GLFWwindow* window_ptr;
        std::vector<myGl::Canvas*> canvasRegions;
        myGl::EventHandler* eventHandler = nullptr;
    };

#endif

#ifndef CANVAS_H
#define CANVAS_H

    //corresponds to an opengl viewport
    class Canvas {
        int width;
        int height;
        int xOffset;
        int yOffset;
        myGl::Window* parent_window;

        //ratios in respect to window size for rescaling
        float widthRatio;
        float heightRatio;
        float xRatio;
        float yRatio;


        friend class Window;

    public:
        Canvas(int width, int height, int x, int y, myGl::Window* parent);
        Canvas(const Canvas& other) = delete;
        Canvas& operator=(const Canvas& other) = default;
        ~Canvas();
        void resize(int width, int height);
        void move(int newX, int newY);
        void clear(float red, float green, float blue, float alpha);
        void drawTo();
    };


[[nodiscard]] myGl::Canvas createFullWindowCanvas(myGl::Window& window);

}
#endif //CANVAS_H
