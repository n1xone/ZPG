#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexData;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float useModelColor;

out vec3 objectColor;

void main()
{
    objectColor = (useModelColor > 0.5) ? vertexData : vec3(0.385, 0.647, 0.812);
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}
