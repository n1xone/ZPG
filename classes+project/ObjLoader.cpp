// Implemented with the assistance of AI.
#include "ObjLoader.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <limits>

std::vector<float> loadTexturedObj(const char* path, float height)
{
    std::ifstream file(path);
    if (!file)
        throw std::runtime_error(std::string("Unable to open OBJ: ") + path);
    std::vector<glm::vec3> positions, normals;
    std::vector<glm::vec2> uvs;
    std::vector<float> vertices;
    glm::vec3 low(std::numeric_limits<float>::max());
    glm::vec3 high(std::numeric_limits<float>::lowest());
    std::string line;
    unsigned lineNumber = 0;
    auto index = [](int value, size_t count) -> size_t
    {
        const long long resolved = value > 0 ? value - 1 : static_cast<long long>(count) + value;
        if (value == 0 || resolved < 0 || resolved >= static_cast<long long>(count))
            throw std::runtime_error("OBJ index out of range");
        return static_cast<size_t>(resolved);
    };
    try
    {
        while (std::getline(file, line))
        {
            ++lineNumber;
            std::istringstream input(line);
            std::string kind;
            input >> kind;
            if (kind == "v" || kind == "vn")
            {
                glm::vec3 v;
                if (!(input >> v.x >> v.y >> v.z))
                    throw std::runtime_error("Invalid vector");
                if (kind == "v")
                {
                    positions.push_back(v);
                    low = glm::min(low, v);
                    high = glm::max(high, v);
                }
                else
                    normals.push_back(v);
            }
            else if (kind == "vt")
            {
                glm::vec2 uv;
                if (!(input >> uv.x >> uv.y))
                    throw std::runtime_error("Invalid UV");
                uvs.push_back(uv);
            }
            else if (kind == "f")
            {
                struct Corner
                {
                    size_t p, t, n;
                };
                std::vector<Corner> face;
                std::string token;
                while (input >> token)
                {
                    if (token[0] == '#')
                        break;
                    std::istringstream corner(token);
                    int p, t, n;
                    char slash1, slash2;
                    if (!(corner >> p >> slash1 >> t >> slash2 >> n) || slash1 != '/' ||
                        slash2 != '/' || corner.peek() != EOF)
                        throw std::runtime_error("Expected position/UV/normal face indices");
                    face.push_back({index(p, positions.size()), index(t, uvs.size()),
                                    index(n, normals.size())});
                }
                if (face.size() < 3)
                    throw std::runtime_error("Face has fewer than three vertices");
                for (size_t i = 1; i + 1 < face.size(); ++i)
                {
                    for (const Corner& c : {face[0], face[i], face[i + 1]})
                    {
                        const auto& p = positions[c.p];
                        const auto& n = normals[c.n];
                        const auto& uv = uvs[c.t];
                        vertices.insert(vertices.end(), {p.x, p.y, p.z, n.x, n.y, n.z, uv.x, uv.y});
                    }
                }
            }
        }
        if (vertices.empty() || high.y <= low.y || height <= 0)
            throw std::runtime_error("Empty model or invalid height");
    }
    catch (const std::exception& error)
    {
        throw std::runtime_error(std::string(path) + ":" + std::to_string(lineNumber) + ": " +
                                 error.what());
    }
    // Center horizontally and place the feet on y=0, preserving proportions.
    const glm::vec3 origin((low.x + high.x) / 2, low.y, (low.z + high.z) / 2);
    const float scale = height / (high.y - low.y);
    for (size_t i = 0; i < vertices.size(); i += 8)
        for (int axis = 0; axis < 3; ++axis)
            vertices[i + axis] = (vertices[i + axis] - origin[axis]) * scale;
    return vertices;
}
