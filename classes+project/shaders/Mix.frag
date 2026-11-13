#version 330 core

in vec2 uv;

uniform sampler2D woodTexture;
uniform sampler2D grassTexture;

out vec4 FragColor;

void main()
{
    vec4 wood = texture(woodTexture, uv);
    vec4 grass = texture(grassTexture, uv);

    FragColor = mix(wood, grass, 0.5);
}
