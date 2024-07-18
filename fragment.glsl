#version 420

out vec4 outColor;

layout (std140, row_major, binding = 0) uniform testBlock
{
    vec3 color;
};

void main()
{
    outColor = vec4(color, 1);
    //outColor = vec4(1, 0.2, 0.4, 1);

}