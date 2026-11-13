#include "Model.h"
#include <stdexcept>

Model::Model(const float* vertices, GLsizeiptr sizeInBytes, int floatsPerVertex,
             int attributeOffset)
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(float), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(float),
                          reinterpret_cast<void*>(attributeOffset * sizeof(float)));
    if (floatsPerVertex == 8)
    {
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              reinterpret_cast<void*>(6 * sizeof(float)));
    }
}

Model::~Model()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}

void Model::draw(unsigned int objectID) const
{
    if (objectID > 255)
        throw std::out_of_range("Object ID exceeds the 8-bit stencil buffer");

    glStencilFunc(GL_ALWAYS, objectID, 0xFF);
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount_);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
}
