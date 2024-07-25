#version 420

in vec3 color;
out vec4 outColor;

void main()
{
    outColor = vec4(color, 1);
}