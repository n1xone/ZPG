#pragma once
#include <glad/gl.h>

class Model
{
public:
    // Vertices contain position and color (or normal), optionally followed by normals.
    Model(const float* vertices, GLsizeiptr sizeInBytes, int floatsPerVertex = 6,
          int attributeOffset = 3);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void draw(unsigned int objectID) const;

private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLsizei vertexCount_ = 0;
};
