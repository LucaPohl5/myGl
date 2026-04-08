#ifndef MODEL_H
#define MODEL_H
#include <glm/glm.hpp>
#include <span>
#include <map>
#include <assimp/scene.h>
#include "Shader.h"
#include "Renderer.h"



namespace myGl {

class Mesh; //forward declaration
class Renderer;

enum class TextureType {
    Diffuse,
    Specular
};
struct Texture {
    uint32_t id;
    TextureType type;
    std::string path;
};


/**
 * manages textures for a scene
 */
class TextureManager {
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
public:
    std::shared_ptr<Texture> load(const std::string& path, TextureType type);
    std::weak_ptr<Texture> get(const std::string& path);
    void assignTexture(Mesh* mesh, const std::string& path, TextureType type);

    ~TextureManager();
};


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

class Mesh {
    uint32_t VAO, VBO, EBO;
    uint32_t indicesCount;

    //for texture handling
    std::vector<std::shared_ptr<Texture>> textures;
    friend void TextureManager::assignTexture(Mesh* mesh, const std::string& path, TextureType type);

    //Data to vertices and face indices
    MeshData* meshData = nullptr;

    //access for drawing mesh
    friend class Renderer;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
    Mesh(const Mesh& other) = delete;
    Mesh& operator=(const Mesh& other) = delete;
    Mesh(Mesh&& other) noexcept ;
    Mesh& operator=(Mesh&& other) noexcept ;
    ~Mesh();
    void freeMeshData();


    [[nodiscard]] uint32_t getVAO() const{return VAO;};
    [[nodiscard]] uint32_t getVBO() const{return VBO;};
};


std::unique_ptr<Mesh> createCubeMesh(float scale);

std::unique_ptr<Mesh> createSphereMesh(float radius, int32_t stackCount = 35, int32_t sectorCount = 35);

std::vector<glm::vec3> calcMeshNormals(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices);





class Model {
    std::vector<Mesh> meshes;
    std::string directory;

    //needs private access for drawing
    friend class Renderer;

public:
    Model(const std::string& path, TextureManager& textManager);
    explicit Model(Mesh& mesh);
    void loadAINode(aiNode* node, const aiScene* scene, TextureManager& textManager);
    void loadMeshFromAI(aiMesh* mesh, const aiScene* scene, TextureManager& textManager);
    void loadTexturesFromAI(aiMaterial *mat, Mesh* mesh, TextureManager& textManager);
};

struct ScenegraphNode {
    glm::mat4 parentTransformation = glm::mat4{1.f};
    std::vector<std::unique_ptr<ScenegraphNode>> children = std::vector<std::unique_ptr<ScenegraphNode>>{};
    std::string name;

    std::string model; //model vector of scene
    bool hasModel = false; //if this node contains a model

    explicit ScenegraphNode(std::string name) : name{std::move(name)} {};
    bool findNode(std::string_view nodeName);
    bool findNodeWithFunction(std::string_view nodeName, const std::function<bool(ScenegraphNode *node)>& func);
    void printGraph();
};

class Scene {
    TextureManager textManager = TextureManager{};
    std::unordered_map<std::string, Model> models = std::unordered_map<std::string, Model>{}; //models with names as keys

    std::unique_ptr<ScenegraphNode> root = std::make_unique<ScenegraphNode>("GLOBAL SPACE"); //root node = global space

    friend class Renderer;

public:
    Scene() = default;
    Scene(const Scene& other) = delete;
    Scene& operator=(const Scene& other) = delete;

    void printGraph(){root->printGraph();} //prints scenegraph

    void loadModel(const std::string& path);
    void addChild(std::string_view parentName, std::string& childName, glm::mat4 parentTransformation = glm::mat4(1.f));
    void addModel(std::string_view nodeName, const std::string& modelName);
    std::unique_ptr<ScenegraphNode>& getRoot(){return root;};
};





} // myGl

#endif //MODEL_H
