#version 450 core

in vec3 vNormal;
in vec3 vFragPos;

uniform vec3 uColor;
uniform vec3 uLightDir;
uniform bool uWireframe;
uniform bool uSelected;

out vec4 FragColor;

void main() {
    if (uWireframe) {
        FragColor = vec4(uColor, 1.0);
    } else {
        vec3 norm = normalize(vNormal);
        vec3 lightDir = normalize(-uLightDir);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 ambient = 0.3 * uColor;
        vec3 diffuse = diff * uColor;
        vec3 result = ambient + diffuse;
        
        if (uSelected) {
            result = mix(result, vec3(1.0, 0.5, 0.0), 0.3);
        }
        
        FragColor = vec4(result, 1.0);
    }
}
