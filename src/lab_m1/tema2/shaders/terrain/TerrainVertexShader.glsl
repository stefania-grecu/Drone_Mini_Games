#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 fragPosition;

float noise(vec2 uv) {
    return fract(sin(dot(uv.xy, vec2(10, 25))) * 100);
}

void main() {
    vec3 newPosition = position;

    float frequency = 0.1;
    float amplitude = 0.3;
    float height = noise(position.xz * frequency) * amplitude;
    newPosition.y += height;

    fragPosition = newPosition;

    gl_Position = Projection * View * Model * vec4(newPosition, 1.0);
}