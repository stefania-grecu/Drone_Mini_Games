#version 330 core

in vec3 fragPosition;

out vec4 fragColor;

void main() {
    float height = fragPosition.y;

    vec3 color;
    if (height < 0.3) {
        color = mix(vec3(0.0, 0.3, 0.0), vec3(0.5, 0.35, 0.2), height / 0.3); // Verde spre maro
    } else if (height < 0.6) {
        color = mix(vec3(0.5, 0.35, 0.2), vec3(1.0, 0.8, 0.2), (height - 0.3) / 1.0); // Maro spre galben
    } else {
        color = vec3(1.0, 0.8, 0.2); // Galben
    }

    fragColor = vec4(color, 1.0);
}