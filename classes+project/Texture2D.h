#pragma once

#include <glad/gl.h>

class Texture2D
{
public:
    explicit Texture2D(const char* path);
    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    void bind(unsigned int unit) const;

private:
    GLuint id_ = 0;
};
