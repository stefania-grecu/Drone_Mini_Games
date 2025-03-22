#version 330

// Input
// TODO(student): Get values from vertex shader
uniform vec3 objectColor;

// Output
out vec4 out_color;


void main()
{
    // TODO(student): Write pixel out color
    // out_color = vec4(1);
    out_color = vec4(objectColor, 1);

}
