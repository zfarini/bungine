cbuffer constants: register(b0)
{
	row_major float4x4 view;
	row_major float4x4 projection;
	row_major float4x4 model;
	row_major float4x4 normal_transform;
	row_major float4x4 light_transform;
	row_major float4x4 bones[96];
	float3 camera_p;
	float diffuse_factor;
	float specular_factor;
	float specular_exponent_factor;
	int skinned;
	int has_normal_map;
};

struct vs_in {
    float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	int4 indices : BLENDINDICES;
	float4 weights : BLENDWEIGHT;
};

struct vs_out {
    float4 position_clip : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;

	float3 world_p : POS;
	float3 object_p : OBJECTP;
};

vs_out vs_main(vs_in input)
{
	vs_out output;

	float4x4 transform;
	if (skinned) {
		transform = 
			(mul(input.weights.x, bones[input.indices.x]) +
			 mul(input.weights.y, bones[input.indices.y]) +
			 mul(input.weights.z, bones[input.indices.z]) +
			 mul(input.weights.w, bones[input.indices.w]));
	}
	else
		transform = model;
	float4 world_p = mul(transform, float4(input.position, 1));
	output.world_p = world_p.xyz;
	output.position_clip = mul(projection, mul(view, world_p));
	output.uv = input.uv;

	//output.normal = mul(normal_transform, float4(input.normal, 0)).xyz;
	output.normal = input.normal;
	output.object_p = input.position;

	return output;
}

Texture2D diffuse_tex : register(t0);
Texture2D specular_tex : register(t1);
Texture2D normal_tex : register(t2);
Texture2D specular_exponent_tex : register(t3);

Texture2D<float> shadow_map : register(t4);

SamplerState mysampler : register(s0);

float4 ps_main(vs_out input) : SV_TARGET
{
	//float2 uv = input.position_clip.xy / float2(1280, 720);
	//return shadow_map.Sample(mysampler, uv);
	
	//return float4(uv, 0, 1);

	
	float3 normal;

	if (has_normal_map)
	{
		float2 ds = ddx(input.uv);
		float2 dt = ddy(input.uv);

		float3 dx = ddx(input.object_p);
		float3 dy = ddy(input.object_p);

		float a = dt.y;
		float b = -dt.x;
		float c = -ds.y;
		float d = ds.x;

		float3 T = dx * a + dy * c;
		float3 B = dx * b + dy * d;
		float3 N = input.normal;

		T = normalize(mul(normal_transform, float4(T, 0)).xyz);
		B = normalize(mul(normal_transform, float4(B, 0)).xyz);
		N = normalize(mul(normal_transform, float4(N, 0)).xyz);

		normal = (normal_tex.Sample(mysampler, input.uv).xyz) * 2 - float3(1, 1, 1);	
		normal = normalize(mul(transpose(float3x3(T, B, N)),  normal));
	}
	else
		normal = normalize(mul(normal_transform, float4(input.normal, 0)).xyz);

	//return pow(float4(0.5f * (normal + float3(1, 1, 1)), 1), 2.2);
	//return specular_tex.Sample(mysampler, input.uv);
	//return specular_exponent_tex.Sample(mysampler, input.uv);
	float shininess_exponenet = max(0.001f, specular_exponent_factor * specular_exponent_tex.Sample(mysampler, input.uv).r);
	float3 color = diffuse_tex.Sample(mysampler, input.uv).rgb;

	//return float4(color, 1);	
	//return float4(color, 1);

	float3 light_pos[2] = {
		float3(4, 0, 3),
		float3(-4, 0, 3)
	};
	float3 specular_color = specular_tex.Sample(mysampler, input.uv).rgb;

	float3 to_camera = normalize(camera_p - input.world_p);

	float3 result = 0;
	
	{
		float3 to_light = -normalize(float3(-1, 0, -0.8));
		float4 p = mul(light_transform, float4(input.world_p, 1));
		p.xyz /= p.w;
		// TODO: I really don't understand why we have to do this?
		p.y *= -1;
		p.xy = 0.5f * (p.xy + float2(1, 1));
	
		float z = shadow_map.Sample(mysampler, p.xy).r;
		//float z = shadow_map[p.xy];

		// TODO: maybe use a sampler with a black border
		// or check some of the other sampling functions
		if (p.z < z + 0.005
		&& p.x >= 0 && p.x < 1 &&
		p.y >= 0 && p.y < 1
		&& p.z >= 0)
		{
			float diffuse = max(0, dot(to_light, normal));
		
			float3 specular = specular_color * pow(max(0, dot(reflect(-to_light, normal), to_camera)), shininess_exponenet);
			result += diffuse_factor*color * diffuse  + specular_factor*specular;
		}
	}
	result *= 0.3;

	//result = color;
#if 1
	result += 0.1f * color;
	for (int i = 0; i < 2; i++) {
		float3 to_light = normalize(light_pos[i] - input.world_p);
		float diffuse = max(0, dot(to_light, normal));
		
		float3 specular = specular_color * pow(max(0, dot(reflect(-to_light, normal), to_camera)), shininess_exponenet);
		//specular = 0;
		//specular = 0;	
		//diffuse = 0;
		//specular *= specular;
		result += (diffuse_factor*color * diffuse  + specular_factor*specular) * 1.f / length(light_pos[i] - input.world_p) ;
	}
#endif
	//result = specular;
	//color = 0.5f*(normal + float3(1, 1, 1));

	return float4(result, 1);
}

struct lines_vs_in {
	float3 position : POSITION;
	float3 color : COLOR;
};

struct lines_vs_out {
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

lines_vs_out lines_vs_main(lines_vs_in input)
{
	lines_vs_out output;

	output.position = mul(projection, mul(view, float4(input.position, 1)));
	output.color = input.color;
	return output;
}

float4 lines_ps_main(lines_vs_out input) : SV_TARGET0
{
	return float4(input.color, 1);
}