#version 420

layout (location = 0) in vec3 vertexP;

void main()
{
	gl_Position = vec4(vertexP, 1);
}