#version 330 core

in vec3 originalColor;

out vec4 FragColor;

void main()
{
    vec3 color = originalColor;

    // Yellow -> light blue
    if (color.r > 0.75 && color.g > 0.75 && color.b < 0.35)
    {
        color = vec3(0.45, 0.85, 1.0);
    }

    // Red -> red (second hand)
    else if (color.r > 0.75 && color.g < 0.3 && color.b < 0.3)
    {
        color = vec3(1.0, 0.0, 0.0);
    }

    // Green or blue -> black (hour and minute hands)
    else if (
        (color.g > 0.6 && color.r < 0.3 && color.b < 0.4) ||
        (color.b > 0.6 && color.r < 0.3 && color.g < 0.4)
    )
    {
        color = vec3(0.0, 0.0, 0.0);
    }

    FragColor = vec4(color, 1.0);
}