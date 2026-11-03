#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;

out vec4 fragColor;

void main()
{
    vec3 bushColor = vec3(0.10, 0.48, 0.10);

    vec3 N = normalize(worldNormal);
    vec3 L = normalize(lightPosition - worldPosition);

    float diffuse = max(dot(N, L), 0.0);

    float distance = length(lightPosition - worldPosition);

    float attenuation = 1.0 / (lightConstant + lightLinear * distance + lightQuadratic * distance * distance);

    vec3 ambient = 0.02 * bushColor;

    vec3 lighting = lightIntensity * diffuse * attenuation * lightColor * bushColor;

    fragColor = vec4(ambient + lighting, 1.0);
}