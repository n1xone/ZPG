#version 330 core

uniform vec3 fragmentColor = vec3(1.0, 1.0, 0.0);

out vec4 FragColor;

void main()
{
    FragColor = vec4(fragmentColor, 1.0);
}
