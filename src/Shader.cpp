#include "Shader.h"
#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace myGl {
/**
 * creates a new glsl shader program from the give files, which can be used with .use()
 * @param vertex_shader_path path to vertex shader source code
 * @param fragment_shader_path path to fragment shader source code
 */
Shader::Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
    id = glCreateProgram();

    std::ifstream vertex_input, fragment_input;

    vertex_input.open(vertex_shader_path);
    fragment_input.open(fragment_shader_path);
    if (!vertex_input.is_open()) {
        std::cerr << "Failed to open file: " << vertex_shader_path << std::endl;
        return;
    }
    if (!fragment_input.is_open()) {
        std::cerr << "Failed to open file: " << fragment_shader_path << std::endl;
        return;
    }
    //load shader file content first into string and the into const char*
    std::string vertex_shader{std::istreambuf_iterator<char>(vertex_input), std::istreambuf_iterator<char>()};
    std::string fragment_shader{std::istreambuf_iterator<char>(fragment_input), std::istreambuf_iterator<char>()};
    const char* v_shader_content = vertex_shader.c_str();
    const char* f_shader_content = fragment_shader.c_str();

    vertex_input.close();
    fragment_input.close();

    int32_t success;
    char infoLog[512];
    //load vertex shader
    uint32_t v_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v_shader, 1, &v_shader_content, nullptr);
    glCompileShader(v_shader);
    //check if compilation was successful
    glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(v_shader, 512, nullptr, infoLog);
        std::cout << "Vertex shader compilation failed: " << std::endl;
        std::cout << infoLog << std::endl;
    }

    //load fragment shader
    uint32_t f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f_shader, 1, &f_shader_content, nullptr);
    glCompileShader(f_shader);
    glGetShaderiv(f_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(f_shader, 512, nullptr, infoLog);
        std::cout << "Fragment shader compilation failed: " << std::endl;
        std::cout << infoLog << std::endl;
    }

    //link shader with program
    glAttachShader(id, v_shader);
    glAttachShader(id, f_shader);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, nullptr, infoLog);
        std::cout << "Shader linking failed: " << std::endl;
        std::cout << infoLog << std::endl;
    }

    //delete shaders
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

}

//function to use this shader program for rendering
void Shader::use() const {
    glUseProgram(id);
}


//set methods to set uniforms in glsl
void Shader::setBool(std::string_view name, bool value) {
    glUniform1i(glGetUniformLocation(id, std::string(name).c_str()), static_cast<int>(value));
}

void Shader::setInt(std::string_view name, int value) {
    glUniform1i(glGetUniformLocation(id, std::string(name).c_str()), value);
}

void Shader::setFloat(std::string_view name, float value) {
    glUniform1f(glGetUniformLocation(id, std::string(name).c_str()), value);
}

void Shader::setMat4(std::string_view name, glm::mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(id, std::string(name).c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat3(std::string_view name, glm::mat3 value) {
    glUniformMatrix3fv(glGetUniformLocation(id, std::string(name).c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec4(std::string_view name, glm::vec4 value) {
    glUniform4fv(glGetUniformLocation(id, std::string(name).c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(std::string_view name, glm::vec3 value) {
    glUniform3fv(glGetUniformLocation(id, std::string(name).c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec2(std::string_view name, glm::vec2 value) {
    glUniform2fv(glGetUniformLocation(id, std::string(name).c_str()), 1, glm::value_ptr(value));
}
}