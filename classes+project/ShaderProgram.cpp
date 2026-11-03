#include "ShaderProgram.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

GLuint ShaderProgram::compile(GLenum type, const char* path)
{
    std::ifstream file(path);
    if (!file)
        throw std::runtime_error(std::string("Unable to open shader: ") + path);

    const std::string code((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

    const char* source = code.c_str();
    GLuint shader = glCreateShader(type);

    if (!shader)
        throw std::runtime_error("Unable to create shader");

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);


    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        glDeleteShader(shader);
        throw std::runtime_error(std::string("Shader failed: ") + path + "\n" + log);
    }
    return shader;
}

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    GLuint vertex = compile(GL_VERTEX_SHADER, vertexPath);
    GLuint fragment = 0;

    try {
        fragment = compile(GL_FRAGMENT_SHADER, fragmentPath);
        program_ = glCreateProgram();

        if (!program_)
            throw std::runtime_error("Unable to create shader program");

        glAttachShader(program_, vertex);
        glAttachShader(program_, fragment);
        glLinkProgram(program_);

        GLint success;
        glGetProgramiv(program_, GL_LINK_STATUS, &success);

        if (!success) {
            char log[1024] = {};
            glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
            throw std::runtime_error(std::string("Shader program link failed:\n") + log);
        }
    }catch (...) {
        glDeleteShader(vertex);
        if (fragment) glDeleteShader(fragment);
        if (program_) glDeleteProgram(program_);
        throw;
    }
    glDetachShader(program_, vertex);
    glDetachShader(program_, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(program_); }

void ShaderProgram::use() const { glUseProgram(program_); }

void ShaderProgram::unuse() { glUseProgram(0); }

GLint ShaderProgram::uniformLocation(const char* name) const
{
    GLint location = glGetUniformLocation(program_, name);
    if (location == -1)
        throw std::runtime_error(std::string("Uniform not found: ") + name); //end of the program
    return location;
}

void ShaderProgram::setUniform(const char* name, float value) const
{
    const GLint location = uniformLocation(name);
    use();
    glUniform1f(location, value);
    unuse();
}

void ShaderProgram::setUniform(const char* name, float x, float y, float z) const
{
    const GLint location = uniformLocation(name);
    use();
    glUniform3f(location, x, y, z);
    unuse();
}

void ShaderProgram::setUniform(const char* name, const glm::mat4& matrix) const
{
    const GLint location = uniformLocation(name);
    use();
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    unuse();
}