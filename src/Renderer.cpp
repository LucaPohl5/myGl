#include "Renderer.h"
#include <iostream>

namespace myGl {

void Renderer::setGlFlags() const {
    if (useZBuffer) {
        glEnable(GL_DEPTH_TEST);
    }else {
        glDisable(GL_DEPTH_TEST);
    }
    //add other flags here
}

/**
 * draws a mesh to the currently picked viewport
 * @param shader shader to use
 * @param mesh mesh which to draw
 */
void Renderer::drawMesh(Shader& shader, Mesh& mesh) {
    shader.use();
    //setup textures
    int32_t diff_count = 1;
    int32_t spec_count = 1;
    for (uint32_t i = 0; i < mesh.textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        Texture texture= *mesh.textures[i];
        std::string text_name;
        if (texture.type == TextureType::Diffuse) {
            text_name = "textures_diffuse" + std::to_string(diff_count);
            diff_count++;
        }
        if (texture.type == TextureType::Specular) {
            text_name = "textures_specular" + std::to_string(spec_count);
            spec_count++;
        }
        shader.setInt(text_name, i);
        glBindTexture(GL_TEXTURE_2D, texture.id);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int32_t>(mesh.indicesCount), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}


/**
 * draws a model to current viewport
 * @param shader shader to use (same for every mesh)
 * @param model model to draw
 */
void Renderer::drawModel(Shader& shader, Model& model) {
    for (Mesh& mesh : model.meshes) {
        drawMesh(shader, mesh);
    }
}

//draws mesh to specific canvas
void Renderer::drawMesh(Shader& shader, Mesh& mesh, Canvas& canvas) {
    canvas.drawTo();
    setGlFlags();
    drawMesh(shader, mesh);
}

//draws mesh to specific canvas
void Renderer::drawModel(Shader& shader, Model& model, Canvas& canvas) {
    canvas.drawTo();
    setGlFlags();
    drawModel(shader, model);
}


void Renderer::drawScene(Shader& shader, Scene& scene, Canvas& canvas) {
    canvas.drawTo();
    setGlFlags();
    drawNode(shader, scene, scene.getRoot(), glm::mat4{1.});
}


//draws scenegraph started by node recursivly
void Renderer::drawNode(Shader& shader, Scene& scene, std::unique_ptr<ScenegraphNode>& node, glm::mat4 parentGlobalTransform) {
    glm::mat4 currentGlobalTransform = parentGlobalTransform * node->parentTransformation;
    if (node->hasModel) {
        shader.setMat4("model", currentGlobalTransform);
        drawModel(shader, scene.models.at(node->model));
    }
    for (auto& child : node->children) {
        drawNode(shader, scene, child, currentGlobalTransform);
    }
}










} // myGl