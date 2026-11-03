#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColor;

uniform mat4 modelMatrix = mat4(1.0);
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

uniform float timeSeconds;

out vec3 originalColor;

void main()
{
    originalColor = vertexColor;

    vec3 pos = position;
    float angle = 0.0;

    const float PI = 3.14159265359;

    // Second hand - red
    if (vertexColor.r > 0.75 && vertexColor.g < 0.3 && vertexColor.b < 0.3)
    {
        angle = -timeSeconds * 2.0 * PI / 60.0;   //60 second
    }

    // Minute hand - green
    else if (vertexColor.g > 0.6 && vertexColor.r < 0.3 && vertexColor.b < 0.4)
    {
        angle = -timeSeconds * 2.0 * PI / 3600.0; //1 hour
    }

    // Hour hand - blue
    else if (vertexColor.b > 0.6 && vertexColor.r < 0.3 && vertexColor.g < 0.4)
    {
        angle = -timeSeconds * 2.0 * PI / 43200.0; //12 hours
    }

    // Rotate around the center of the clock
    vec2 pivot = vec2(0.0, 0.0);

    float c = cos(angle);
    float s = sin(angle);

    vec2 p = pos.xy - pivot;

    pos.xy = vec2(
        p.x * c - p.y * s,
        p.x * s + p.y * c
    ) + pivot;

    gl_Position =
        projectionMatrix *
        viewMatrix *
        modelMatrix *
        vec4(pos, 1.0);
}