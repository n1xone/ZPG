
#version 330 core

in vec3 texDirection;

uniform samplerCube skyTexture;

out vec4 FragColor;

void main()
{
    FragColor = texture(skyTexture, normalize(texDirection));
}
