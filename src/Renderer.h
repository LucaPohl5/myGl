#ifndef RENDERER_H
#define RENDERER_H

#include "Model.h"
#include "Window.h"

namespace myGl {

class Mesh;
class Model;
struct ScenegraphNode;
class Scene;


class Renderer {

    //flags
    void setGlFlags() const;
    bool useZBuffer = true;
    //private subcalls for drawing
    void drawMesh(Shader& shader, Mesh& mesh);
    void drawModel(Shader& shader, Model& model);
    void drawNode(Shader& shader, Scene& scene, std::unique_ptr<ScenegraphNode>& node, glm::mat4 parentGlobalTransform);
public:
    Renderer() = default;

    void drawMesh(Shader& shader, Mesh& mesh, Canvas& canvas);
    void drawModel(Shader& shader, Model& model, Canvas& canvas);
    void drawScene(Shader& shader, Scene& scene, Canvas& canvas);
};

} // myGl

#endif //RENDERER_H
