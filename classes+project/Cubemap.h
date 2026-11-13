#pragma once

#include <glad/gl.h>
#include <array>
#include <string>

class Cubemap
{
public:
    explicit Cubemap(const std::array<std::string, 6>& faces);
    ~Cubemap();

    Cubemap(const Cubemap&) = delete;
    Cubemap& operator=(const Cubemap&) = delete;

    void bind(unsigned int unit) const;

private:
    GLuint id_ = 0;
};
