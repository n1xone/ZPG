
#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

class ShaderProgram
{
public:
    ShaderProgram(const char* vertexPath, const char* fragmentPath);
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void use() const;
    static void unuse();

    void setUniform(const char* name, float value) const;
    void setUniform(const char* name, float x, float y, float z) const;
    void setUniform(const char* name, const glm::mat4& matrix) const;

private:
    static GLuint compile(GLenum type, const char* path);
    GLint uniformLocation(const char* name) const;

    GLuint program_ = 0;
};
