
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 texDirection;

void main()
{
    texDirection = position;

    mat4 rotationView = mat4(mat3(viewMatrix));

    vec4 clipPos = projectionMatrix *
                   rotationView *
                   vec4(position, 1.0);

    gl_Position = clipPos.xyww;
}
