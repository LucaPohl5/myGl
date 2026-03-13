#ifndef SHADER_H
#define SHADER_H

#include <cstdlib>
#include <string_view>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace myGl {

class Shader {
    uint32_t id;

public:
    Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;
    ~Shader() {glDeleteProgram(id);}
    void use() const;

    //set methods to set uniforms in shader program
    void setBool(std::string_view name, bool value);
    void setInt(std::string_view name, int value);
    void setFloat(std::string_view name, float value);
    void setMat4(std::string_view name, glm::mat4 value);
    void setMat3(std::string_view name, glm::mat3 value);
    void setVec4(std::string_view name, glm::vec4 value);
    void setVec3(std::string_view name, glm::vec3 value);
    void setVec2(std::string_view name, glm::vec2 value);
};

}

#endif
