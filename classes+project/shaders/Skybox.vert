#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 texDirection;

void main()
{
    texDirection = position;

    // Remove camera translation
    mat4 rotationView = mat4(mat3(viewMatrix));

    vec4 clipPosition =
        projectionMatrix * rotationView * vec4(position, 1.0);

    // Put skybox at maximum depth
    gl_Position = clipPosition.xyww;
}
