#version 330 core

in vec3 vertexNormal;

out vec4 FragColor;

uniform vec3 fragmentColor;

void main()
{
    FragColor = vec4( (vertexNormal * 0.5 + 0.5) * fragmentColor, 1.0 );
}