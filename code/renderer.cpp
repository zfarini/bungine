void push_line(v3 a, v3 b, v3 color = V3(1))
{

}

void push_cube_outline(v3 center, v3 radius, v3 color = V3(1))
{

}

void push_triangle_outline(v3 a, v3 b, v3 c, v3 color = V3(1))
{

}

void push_ellipsoid_outline(v3 center, v3 radius, v3 color = V3(1))
{
	
}

struct Constants
{
	mat4 view;
	mat4 projection;
	mat4 model;
	//mat4 normal_transform;
	mat4 light_transform;
	mat4 bones[96];

	v3 camera_p;
	v3 player_p;
	float diffuse_factor;

	float specular_factor;
	float specular_exponent_factor;
	int skinned;
	int has_normal_map;
};