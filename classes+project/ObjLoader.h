// Implemented with the assistance of AI.
#pragma once
#include <vector>

// Loads textured OBJ faces as interleaved position, normal, UV (8 floats).
// Supports v/vt/vn indices and fan triangulation of convex polygons.
std::vector<float> loadTexturedObj(const char* path, float height);
