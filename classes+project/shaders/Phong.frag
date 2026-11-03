#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 objectColor;
in vec3 cameraPosition;

uniform vec3 lightPosition;
uniform float useFlatNormal;
out vec4 FragColor;

void main()
{
    vec3 N = normalize(worldNormal);
    if (useFlatNormal > 0.5)
        N = normalize(cross(dFdx(worldPosition), dFdy(worldPosition)));

    vec3 L = normalize(lightPosition - worldPosition);
    vec3 V = normalize(cameraPosition - worldPosition);

    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 ambient = 0.18 * objectColor;
    vec3 diffuse = 0.82 * diffuseFactor * objectColor;

    float specularFactor = 0.0;
    if (diffuseFactor > 0.0)
    {
        vec3 R = reflect(-L, N);
        specularFactor = pow(max(dot(V, R), 0.0), 32.0);
    }

    //vec3 R = reflect(-L, N);
    //float specularFactor = pow(max(dot(V, R), 0.0), 1.0); 

    vec3 specular = 0.75 * specularFactor * vec3(1.0);
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
