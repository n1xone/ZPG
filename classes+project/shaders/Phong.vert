#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexData;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float useModelColor;       // 1: position, color, normal (9 floats)
uniform float useSphericalNormal;  // 1: derive normal from the sphere's local position

out vec3 worldPosition;
out vec3 worldNormal;
out vec3 objectColor;
out vec3 cameraPosition;

void main()
{
    vec4 world = modelMatrix * vec4(position, 1.0);
    worldPosition = world.xyz;
    vec3 localNormal = (useModelColor > 0.5) ? vec3(0.0, 0.0, 1.0) : vertexData;

    if (useSphericalNormal > 0.5)
        localNormal = normalize(position);

    worldNormal = normalize(transpose(inverse(mat3(modelMatrix))) * localNormal);
    //worldNormal = localNormal; //for testing problem of normal

    objectColor = (useModelColor > 0.5) ? vertexData : vec3(0.385, 0.647, 0.812);
    cameraPosition = vec3(inverse(viewMatrix)[3]);
    
    gl_Position = projectionMatrix * viewMatrix * world;
}
