#ifndef CANVAS_H
#define CANVAS_H

namespace myGl {

class Window;

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

} //namespace myGl

#endif //CANVAS_H
