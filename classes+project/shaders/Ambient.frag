#version 330 core
in vec3 objectColor;
out vec4 fragColor;
void main()
{
    // A constant ambient term, independent of light direction.
    fragColor = vec4(objectColor, 1.0);
}
