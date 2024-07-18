#version 420

layout (std140, row_major, binding = 0) uniform Constants
{
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 light_transform;
	mat4 bones[96];

	vec3 camera_p;
	vec3 player_p;
	float diffuse_factor;
	float specular_factor;
	float specular_exponent_factor;
	int skinned;
	int has_normal_map;
};

out vec4 outColor;

in vec2 uv;
in vec3 inormal;
in vec3 world_p;
in vec3 object_p;

layout (binding = 0) uniform sampler2D diffuse_tex;
layout (binding = 1) uniform sampler2D specular_tex;
layout (binding = 2) uniform sampler2D normal_map_tex;
layout (binding = 3) uniform sampler2D specular_exponent_tex;
layout (binding = 4) uniform sampler2D shadow_map_tex;

in mat4 normal_transform;

void main()
{
    vec3 normal;

    if (has_normal_map != 0)
	{
		vec2 ds = dFdx(uv);
		vec2 dt = dFdy(uv);

		vec3 dx = dFdx(object_p);
		vec3 dy = dFdy(object_p);

		float a = dt.y;
		float b = -dt.x;
		float c = -ds.y;
		float d = ds.x;

		vec3 T = dx * a + dy * c;
		vec3 B = dx * b + dy * d;
		vec3 N = inormal;

		T = normalize((normal_transform * vec4(T, 0)).xyz);
		B = normalize((normal_transform * vec4(B, 0)).xyz);
		N = normalize((normal_transform * vec4(N, 0)).xyz);

		normal = (texture(normal_map_tex, uv).xyz) * 2 - vec3(1);
		normal = normalize(mat3(T, B, N) * normal);
	}
	else
		normal = normalize((normal_transform * vec4(inormal, 0)).xyz);

    //outColor = vec4(normal * 0.5f + 0.5f, 1);
    //return ;

    vec3 color = texture(diffuse_tex, uv).rgb;
	
    float shininess_exponenet = max(0.001f, specular_exponent_factor * texture(specular_exponent_tex, uv).r);
    vec3 specular_color = texture(specular_tex, uv).rgb;
	vec3 to_camera = normalize(camera_p - world_p);


    vec3 result = vec3(0, 0, 0);
    {
		vec3 to_light = -normalize(vec3(-1, 0, -1));
		vec4 p = light_transform * vec4(world_p, 1);
		p.xyz /= p.w;
		// TODO: I really don't understand why we have to do this?
		//p.y *= -1;
		p.xyz = 0.5f * (p.xyz + 1);
	
		float z = texture(shadow_map_tex, p.xy).r;

       
		//float z = shadow_map[p.xy];

		// TODO: maybe use a sampler with a black border
		// or check some of the other sampling functions
		if (p.z < z + 0.005
		&& p.x >= 0 && p.x < 1 &&
		p.y >= 0 && p.y < 1)
		{
			float diffuse = max(0, dot(to_light, normal));
		
			vec3 specular = specular_color * pow(max(0.f, dot(reflect(-to_light, normal), to_camera)), shininess_exponenet);
			result += diffuse_factor*color * diffuse  + specular_factor*specular;
		}
        result *= 0.3f;
	}

    result += 0.2f * color;
    //return ;
#if 1
	vec3 light_pos[2] = {
		player_p,
		vec3(0, 2, 4)
	};
	vec3 light_color[2] = {
		vec3(0.8, 0.2, 0.3),
		vec3(0.1, 0.9, 0.4)
	};
	for (int i = 0; i < 2; i++) {
		vec3 to_light = normalize(light_pos[i] - world_p);
		float diffuse = max(0.f, dot(to_light, normal));
		
		vec3 specular = specular_color * pow(max(0.f, dot(reflect(-to_light, normal), to_camera)), shininess_exponenet);
		//specular = 0;
		//specular = 0;	
		//diffuse = 0;
		//specular *= specular;
		result += light_color[i] * (diffuse_factor*color * diffuse  + specular_factor*specular) * 1.f / length(light_pos[i] - world_p) ;
	}
#endif
    outColor = vec4(result, 1);
}