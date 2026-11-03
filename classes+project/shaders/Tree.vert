#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 modelMatrix = mat4(1.0);
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

flat out vec3 treeColor;

out vec3 worldPosition;
out vec3 worldNormal;

void main()
{
    if (gl_VertexID > 36443)
    {
        // Leaves
        treeColor = vec3(0.12, 0.55, 0.15);
    }
    else
    {
        // Trunk and branches
        treeColor = vec3(0.40, 0.24, 0.10);
    }

    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    worldPosition = worldPos.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    worldNormal = normalize(normalMatrix * normal);

    gl_Position = projectionMatrix * viewMatrix * worldPos;
}