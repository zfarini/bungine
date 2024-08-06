#version 420

layout (location = 0) in vec3 vposition;
layout (location = 1) in vec3 vnormal;
layout (location = 2) in vec2 vuv;
layout (location = 3) in vec4 weights;
// TODO: try to send them as int again, Idk why it doesn't work only in opengl
layout (location = 4) in vec4 indices;

layout (std140, row_major, binding = 0) uniform Constants
{
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 light_transform;
	mat4 bones[96];

	vec3 camera_p;
	vec3 player_p;
	vec3 tintColor;
	float diffuse_factor;
	float specular_factor;
	float specular_exponent_factor;
	int skinned;
	int has_normal_map;
	int show_normals;
};

out vec2 uv;
out vec3 inormal;
out vec3 world_p;
out mat4 normal_transform;
out vec3 object_p;

void main()
{
	mat4 transform;
#if 1
	if (skinned != 0) {
		transform = 
			(weights.x * bones[int(indices.x)]) +
			(weights.y * bones[int(indices.y)]) +
			(weights.z * bones[int(indices.z)]) +
			(weights.w * bones[int(indices.w)]);
	}
	else
#endif
		transform = model;

#if 1
	vec3 scale = 1 / vec3(dot(transform[0].xyz, transform[0].xyz),
				 dot(transform[1].xyz, transform[1].xyz),
				 dot(transform[2].xyz, transform[2].xyz));
	normal_transform[0] = vec4(transform[0].xyz*scale.x, 0);
	normal_transform[1] = vec4(transform[1].xyz*scale.y, 0);
	normal_transform[2] = vec4(transform[2].xyz*scale.z, 0);
	normal_transform[3] = vec4(0, 0, 0, 1);
#else
	normal_transform = inverse(transpose(transform));
#endif
	world_p = (transform * vec4(vposition, 1)).xyz;
	gl_Position = projection  * view * vec4(world_p, 1);
	object_p = vposition;
	uv = vuv;
	inormal = vnormal;
}
