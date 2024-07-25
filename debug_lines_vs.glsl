#version 420

layout (location = 0) in vec3 vposition;
layout (location = 1) in vec3 vcolor;

layout (std140, row_major, binding = 1) uniform Constants
{
    mat4 mvp;
};

out vec3 color;

void main()
{
    gl_Position = mvp * vec4(vposition, 1);
    color = vcolor;
}