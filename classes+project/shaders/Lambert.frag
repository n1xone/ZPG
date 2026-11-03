#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 objectColor;

uniform vec3 lightPosition;
out vec4 fragColor;

void main()
{
    vec3 N = normalize(worldNormal);
    vec3 L = normalize(lightPosition - worldPosition);
    
    float diffuse = max(dot(N, L), 0.0);
    
    // Lambert task: diffuse only, without ambient or specular.
    fragColor = vec4(objectColor * diffuse, 1.0);
}
