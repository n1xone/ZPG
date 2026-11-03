#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexData;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float useModelColor;

out vec3 worldPosition;
out vec3 worldNormal;
out vec3 objectColor;

void main()
{
    vec4 world = modelMatrix * vec4(position, 1.0);
    worldPosition = world.xyz;
    
    vec3 N = (useModelColor > 0.5) ? vertexNormal : vertexData;
    worldNormal = normalize(transpose(inverse(mat3(modelMatrix))) * N);

    objectColor = (useModelColor > 0.5) ? vertexData : vec3(0.385, 0.647, 0.812);
    
    gl_Position = projectionMatrix * viewMatrix * world;
}
