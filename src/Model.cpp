#include <iostream>
#include <glad/glad.h>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Model.h"
#include "stb/stb_image.h"


namespace myGl {

#define PI 3.14159265358979323846

/**
 * Returns a shared pointer to the texture at path. If texture is not loaded yet, also loads the texture
 * @param path path of the texture image to load (starting from executable)
 * @param type type of texture (e.g. diffuse)
 * @return shared pointer to texture from path
 */
std::shared_ptr<Texture> TextureManager::load(const std::string& path, TextureType type) {
    //check if texture already loaded
    if (textures.find(path) != textures.end()) {
        return std::shared_ptr<Texture>(textures.at(path));
    }

    uint32_t textureID;

    //load texture with stb
    int32_t width, height, nrChannels;
    uint8_t *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLint format = 0;
        if (nrChannels == 1) {
            format = GL_RED;
        }else if (nrChannels == 3) {
            format = GL_RGB;
        }else if (nrChannels == 4){
            format = GL_RGBA;
        }
        if (format == 0) {
            std::cerr << "Invalid texture format! Could not load texture: " << path << std::endl;
            return nullptr;
        }
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load Texture at: " << path << std::endl;
        return nullptr;
    }
    //set textureID params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    std::cout << "Loaded Texture at: " << path << std::endl;

    std::shared_ptr<Texture> texture = std::make_shared<Texture>(Texture{textureID, type, path});
    textures.emplace(path, std::move(texture));
    return std::shared_ptr<Texture>(textures.at(path));
}

std::weak_ptr<Texture> TextureManager::get(const std::string& path) {
    if (textures.find(path) == textures.end()) {
        std::cerr << "Texture at " << path << " does not exist!" << std::endl;
        return {};
    }
    return textures.at(path);
}

/**
 * Assigns texture to a mesh, which then can use the texture for rendering
 * TODO: TextureManager ALWAYS NEEDS LONGER LIFETIME THAN ALL MESHES, WHICH USE TEXTURES OF THIS TEXTURE MANAGER
 * @param mesh mesh to use texture for
 * @param path path of texture
 * @param type type of texture
 */
void TextureManager::assignTexture(Mesh* mesh, const std::string& path, TextureType type) {
    auto textPtr = load(path, type);
    if (textPtr) {
        mesh->textures.push_back(std::move(textPtr));
    }else {
        std::cout << "Failed to assign texture " << path << " to mesh!" << std::endl;
    }
}


/**
 * when deleting a TextureManager, also glDelete all assigned Textures
 */
TextureManager::~TextureManager() {
    for (auto& texture : textures) {
        auto id = texture.second->id;
        glDeleteTextures(1, &id);
    }
}


//----------------------------------------------
//Mesh implementation

/**
 * Create a mesh and upload the mesh data provided to the gpu
 * @param vertices vertices of mesh
 * @param indices indices of vertex triangles of mesh
 */
Mesh::Mesh(std::vector<myGl::Vertex> vertices, std::vector<uint32_t> indices){
    VAO = 0;
    VBO = 0;
    EBO = 0;
    meshData = new MeshData{std::move(vertices), std::move(indices)};
    indicesCount = meshData->indices.size();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    assert(meshData);

    // setup VBO with vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(meshData->vertices.size() * sizeof(Vertex)), &meshData->vertices[0], GL_STATIC_DRAW);


    //setup EBO with indices data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(meshData->indices.size() * sizeof(uint32_t)), &meshData->indices[0], GL_STATIC_DRAW);

    //set vertex attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (offsetof(Vertex, Normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

//when delete a mesh, also delete the buffers on the gpu
Mesh::~Mesh() {
    delete meshData;
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

Mesh::Mesh(Mesh&& other)  noexcept : VAO{other.VAO}, VBO{other.VBO}, EBO{other.EBO}, indicesCount{other.indicesCount}{
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.indicesCount = 0;
    this->meshData = other.meshData;
    other.meshData = nullptr;
    this->textures = std::move(other.textures);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    this->VAO = other.VAO;
    this->VBO = other.VBO;
    this->EBO = other.EBO;
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    this->indicesCount = other.indicesCount;
    other.indicesCount = 0;
    this->textures = std::move(other.textures);
    this->meshData = other.meshData;
    other.meshData = nullptr;
    return *this;
}

//free up the mesh data on the ram
void Mesh::freeMeshData() {
    delete meshData;
    meshData = nullptr;
}


/**
 * create a simple sphere mesh, where stackCount and sectorCount describe the detail (triangle count)
 */
std::unique_ptr<Mesh> createSphereMesh(float radius, int32_t stackCount, int32_t sectorCount) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    float sectorStep = 2.0f * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for(int32_t i = 0; i <= stackCount; ++i){
        stackAngle = PI / 2 - static_cast<float>(i) * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for(int32_t j = 0; j <= sectorCount; ++j){
            sectorAngle = static_cast<float>(j) * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            positions.emplace_back(x, y, z);

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            normals.emplace_back(nx, ny, nz);

            s = static_cast<float>(j) / static_cast<float>(sectorCount);
            t = static_cast<float>(i) / static_cast<float>(stackCount);
            texCoords.emplace_back(s, t);
        }
    }

    std::vector<uint32_t> indices;
    int32_t k1, k2;
    for(int32_t i = 0; i < stackCount; ++i){
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for(int32_t j = 0; j < sectorCount; ++j, ++k1, ++k2){
            if(i != 0){
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if(i != (stackCount-1)){
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    assert(positions.size() == normals.size());
    assert(positions.size() == texCoords.size());
    std::vector<Vertex> vertices;
    vertices.reserve(positions.size());
    for (uint32_t i = 0; i < positions.size(); i++) {
        vertices.emplace_back(Vertex{positions[i], normals[i], texCoords[i]});
    }
    std::unique_ptr<Mesh> sphereMesh = std::make_unique<Mesh>(std::move(vertices), std::move(indices));
    return sphereMesh;
}

/*
 * create a simple cube mesh that is scaled by scale
 */
std::unique_ptr<Mesh> createCubeMesh(float scale) {
    std::vector<glm::vec3> positions = {
        // Front
        {-0.5f, -0.5f,  0.5f},
        { 0.5f, -0.5f,  0.5f},
        { 0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f},

        // Back
        { 0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {-0.5f,  0.5f, -0.5f},
        { 0.5f,  0.5f, -0.5f},

        // Left
        {-0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f, -0.5f},

        // Right
        { 0.5f, -0.5f,  0.5f},
        { 0.5f, -0.5f, -0.5f},
        { 0.5f,  0.5f, -0.5f},
        { 0.5f,  0.5f,  0.5f},

        // Top
        {-0.5f,  0.5f,  0.5f},
        { 0.5f,  0.5f,  0.5f},
        { 0.5f,  0.5f, -0.5f},
        {-0.5f,  0.5f, -0.5f},

        // Bottom
        {-0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f,  0.5f},
        {-0.5f, -0.5f,  0.5f},
    };

    for (auto& pos : positions) {
        pos *= scale;
    }

    const std::vector<glm::vec2> texCoords = {
        // Front
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},

        // Back
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},

        // Left
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},

        // Right
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},

        // Top
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},

        // Bottom
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
    };

    std::vector<uint32_t> indices = {
        // Front
        0, 1, 2, 2, 3, 0,

        // Back
        4, 5, 6, 6, 7, 4,

        // Left
        8, 9,10,10,11, 8,

        // Right
       12,13,14,14,15,12,

        // Top
       16,17,18,18,19,16,

        // Bottom
       20,21,22,22,23,20
    };


    const std::vector<glm::vec3> normals = calcMeshNormals(positions, indices);
    std::vector<myGl::Vertex> vertices;
    vertices.reserve(positions.size());

    assert(positions.size() == normals.size());
    assert(positions.size() == texCoords.size());

    for (uint32_t i = 0; i < positions.size(); i++) {
        vertices.emplace_back(Vertex{positions[i], normals[i], texCoords[i]});
    }
    auto cubeMesh = std::make_unique<Mesh>(std::move(vertices), std::move(indices));
    return cubeMesh;
}

//compute the normal of face with points p1, p2, p3
glm::vec3 computeFaceNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    auto a = p3 - p2;
    auto b = p1 - p2;
    return glm::normalize(glm::cross(a, b));
}

/**
 *calc the normals for a mesh given its vertices and indices
 * @return normals where normals[i] = normal of vertex vertices[i]
 */
std::vector<glm::vec3> calcMeshNormals(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices) {
    auto normals = std::vector<glm::vec3>(vertices.size());

    for (uint32_t i = 0; i < indices.size(); i += 3) {
        glm::vec3 A = vertices[indices[i]];
        glm::vec3 B = vertices[indices[i + 1]];
        glm::vec3 C = vertices[indices[i + 2]];
        glm::vec3 normal = computeFaceNormal(A, B, C);
        normals[indices[i]] += normal;
        normals[indices[i + 1]] += normal;
        normals[indices[i + 2]] += normal;
    }

    for (auto & normal : normals) {
        normal = glm::normalize(normal);
    }
    return normals;
}


//-----------------------------------------------------
//Model implementation


/**
 * loads a model form a given file with assimp
 * @param path path of the model file (e.g. obj)
 * @param textManager where to store the textures for the model (usually the texture manager of the scene model is part of)
 */
Model::Model(const std::string& path, TextureManager& textManager) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ASSIMP error with loading model: " << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    meshes.reserve(scene->mNumMeshes);
    loadAINode(scene->mRootNode, scene, textManager);
}

/*
 * constructs a Model from one Mesh (Mesh is moved and is not usable after this anymore)
 */
Model::Model(Mesh& mesh) {
    meshes.push_back(std::move(mesh));
}



#warning: watch todo
//TODO: add transformations for meshes later to allow for better control of models (also to make recursive loading of nodes make sense)
/**
 * load a scene node from an aiScene and add it to the model
 * @param node node to load
 * @param scene scene where node is part of
 * @param textManager where to store all the textures
 */
void Model::loadAINode(aiNode* node, const aiScene* scene, TextureManager& textManager) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        loadMeshFromAI(mesh, scene, textManager);
    }
    for (int i = 0; i < node->mNumChildren; i++) {
        loadAINode(node->mChildren[i], scene, textManager);
    }
}

/**
 * creates a myGl::Mesh from an aiMesh
 * @param mesh aiMesh to load
 * @param scene aiScene which mesh is part of
 * @param textManager texture Manager to use for texture loading
 */
void Model::loadMeshFromAI(aiMesh* mesh, const aiScene* scene, TextureManager& textManager) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;

    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;
        if (mesh->HasNormals()){
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        }else {
            vertex.Normal = glm::vec3{0};
        }
        if(mesh->mTextureCoords[0]){
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; i++) {
        if (mesh->mFaces[i].mNumIndices != 3) {
            std::cerr << "Invalid model format: Face with more than 3 indices!" << std::endl;
            exit(EXIT_FAILURE);
        }
        uint32_t i1 = mesh->mFaces[i].mIndices[0];
        uint32_t i2 = mesh->mFaces[i].mIndices[1];
        uint32_t i3 = mesh->mFaces[i].mIndices[2];
        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i3);
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    Mesh loadedMesh(std::move(vertices), std::move(indices));
    loadTexturesFromAI(material, &loadedMesh, textManager);
    meshes.emplace_back(std::move(loadedMesh));
}


void Model::loadTexturesFromAI(aiMaterial *mat, Mesh* mesh, TextureManager& textManager) {
    //first load diffuse textures
    for (int i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++) {
        aiString aistr;
        mat->GetTexture(aiTextureType_DIFFUSE, i, &aistr);
        std::string path = directory + '/' + std::string{aistr.C_Str()};
        textManager.assignTexture(mesh, path, TextureType::Diffuse);
    }
    for (int i = 0; i < mat->GetTextureCount(aiTextureType_SPECULAR); i++) {
        aiString aistr;
        mat->GetTexture(aiTextureType_SPECULAR, i, &aistr);
        std::string path = directory + '/' + std::string{aistr.C_Str()};
        textManager.assignTexture(mesh, path, TextureType::Specular);
    }
}


//------------------------------------
//Scene implementation


void Scene::loadModel(const std::string& path) {
    if (models.find(path) != models.end()) { //already loaded
        return;
    }
    models.try_emplace(path, path, textManager);
    std::cout << "Loaded Model: " << path << std::endl;
}

//searches scenegraph for a node with nodeName. returns true if node exists, false if not
bool ScenegraphNode::findNode(std::string_view nodeName) {
    if (name == nodeName) {
        return true;
    }
    for (auto& child : children) {
        if (child->findNode(nodeName)) {
            return true;
        }
    }
    return false;
}

/**
 * finds a node within a scenegraph and runs a function for this node
 * @param nodeName name of the node to find
 * @param func function to run
 * @return true if execution of function was successful, false if function call was not successful or node could not be found
 */
bool ScenegraphNode::findNodeWithFunction(std::string_view nodeName, const std::function<bool(ScenegraphNode *node)>& func) {
    if (name == nodeName) {
        return func(this);
    }
    for (auto& child : children) {
        if (child->findNodeWithFunction(nodeName, func)) {
            return true;
        }
    }
    std::cout << "Could not find node " << nodeName << std::endl;
    return false;
}

void ScenegraphNode::printGraph() {
    std::cout << "Children of " << name << " are: ";
    for (auto& child : children) {
        std::cout << child->name << "  ";
    }
    std::cout << "    Model is: " << model << std::endl;
    for (auto& child : children) {
        child->printGraph();
    }
}


/**
 * adds a new node to the scenegraph (with no model)
 * @param parentName parent node of the new node
 * @param childName  name of new node
 * @param parentTransformation transformation from new node to parent node
 */
void Scene::addChild(std::string_view parentName, std::string& childName, glm::mat4 parentTransformation){
    //lambda for new child creation. if child with same name already exists -> returns false
    auto addChildFunc = [childName, parentTransformation](ScenegraphNode* node) -> bool{
        for (auto& child : node->children) {
            if (child->name == childName) {
                std::cout << "Failed to create new ScenegraphNode! Child with name " << childName << " already exists" << std::endl;
                return false;
            }
        }
        auto newChild = std::make_unique<ScenegraphNode>(childName);
        newChild->parentTransformation = parentTransformation;
        node->children.emplace_back(std::move(newChild));
        return true;
    };


    if (!root->findNode(parentName)) { //parent does not exist
        std::cout << "Could not add child node " << childName << ", because parent node " << parentName << " does not exits!" << std::endl;
        return;
    }
    if (root->findNode(childName)) { //child already exists
        std::cout << "Could not add child node " << childName << ", because it already exists within scenegraph!" << std::endl;
        return;
    }
    if (!root->findNodeWithFunction(parentName, addChildFunc)) {
        std::cout << "Failed to add child node " << childName << " to parent node " << parentName << std::endl;
    }
};


/**
 * adds an EXISTING (already loaded) model to a node. If node already has a model, nothing happens
 */
void Scene::addModel(std::string_view nodeName, const std::string& modelName) {
    if (models.find(modelName) == models.end()) {
        std::cout << "Could not assign Model " << modelName << " to Node " << nodeName << ", because model is not loaded!";
        std::cout << " Model fist needs to be loaded!" << std::endl;
        return;
    }

    auto addModelFunc = [modelName] (ScenegraphNode* node) -> bool {
        if (node->hasModel) {
            std::cout << "Node " << node->name << " already has a Model! Failed to assign Model " << modelName << std::endl;
            return false;
        }
        node->hasModel = true;
        node->model = modelName;
        return true;
    };
    if (!root->findNodeWithFunction(nodeName, addModelFunc)) {
        std::cout << "Could not add model " << modelName << " to Node " << nodeName << std::endl;
    }
}

} // myGl