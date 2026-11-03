#include "Model.h"


Model::Model(const float* vertices, GLsizeiptr sizeInBytes,int floatsPerVertex, int attributeOffset)
    : vertexCount_(static_cast<GLsizei>(sizeInBytes / sizeof(float) / floatsPerVertex))
{
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeInBytes, vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glVertexAttribPointer(
        0, 
        3,
        GL_FLOAT, 
        GL_FALSE, 
        floatsPerVertex * sizeof(float), nullptr);
    glVertexAttribPointer(
        1, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        floatsPerVertex * sizeof(float),
        reinterpret_cast<void*>(attributeOffset * sizeof(float)));
}

Model::~Model()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}

void Model::draw() const  //my draw call
{
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
}
