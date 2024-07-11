#pragma pack_matrix(row_major)

#define BONE_COUNT 96

cbuffer constants: register(b0)
{
	float4x4 view;
	float4x4 projection;
	float4x4 model;
	//row_major float4x4 normal_transform;
	float4x4 light_transform;
	float4x4 bones[96];

	float3 camera_p;
	float3 player_p;
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
	float4 weights : BLENDWEIGHT;
	int indices : BLENDINDICES;
};

struct vs_out {
    float4 position_clip : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;

	float3 world_p : POS;
	float3 object_p : OBJECTP;

	float4x4 normal_transform : NORMAL_TRANSFORM;
};


float4x4 inverse(float4x4 M)
{

	float m[16];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			m[i * 4 + j] = M[i][j];
	}

    float t0 = m[10] * m[15];
    float t1 = m[14] * m[11];
    float t2 = m[6] * m[15];
    float t3 = m[14] * m[7];
    float t4 = m[6] * m[11];
    float t5 = m[10] * m[7];
    float t6 = m[2] * m[15];
    float t7 = m[14] * m[3];
    float t8 = m[2] * m[11];
    float t9 = m[10] * m[3];
    float t10 = m[2] * m[7];
    float t11 = m[6] * m[3];
    float t12 = m[8] * m[13];
    float t13 = m[12] * m[9];
    float t14 = m[4] * m[13];
    float t15 = m[12] * m[5];
    float t16 = m[4] * m[9];
    float t17 = m[8] * m[5];
    float t18 = m[0] * m[13];
    float t19 = m[12] * m[1];
    float t20 = m[0] * m[9];
    float t21 = m[8] * m[1];
    float t22 = m[0] * m[5];
    float t23 = m[4] * m[1];

	float o[16];

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) -
           (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) -
           (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) -
           (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) -
           (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    float d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) -
                (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) -
                (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) -
                (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) -
                (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) -
                (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) -
                (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) -
                 (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) -
                 (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) -
                 (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) -
                 (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) -
                 (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) -
                 (t20 * m[6] + t23 * m[10] + t17 * m[2]));

	float4x4 O;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			O[i][j] = o[i * 4 + j];

    return O;
}


vs_out vs_main(vs_in input)
{
	vs_out output;

	float4x4 transform;
	if (skinned) {
		transform = 
			(mul(input.weights.x, bones[input.indices & 0xFF]) +
			 mul(input.weights.y, bones[(input.indices >> 8) & 0xFF]) +
			 mul(input.weights.z, bones[(input.indices >> 16) & 0xFF]) +
			 mul(input.weights.w, bones[(input.indices >> 24) & 0xff]));
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
	output.normal_transform = transpose(inverse(transform));
	//output.normal_transform = transform;
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

		T = normalize(mul(input.normal_transform, float4(T, 0)).xyz);
		B = normalize(mul(input.normal_transform, float4(B, 0)).xyz);
		N = normalize(mul(input.normal_transform, float4(N, 0)).xyz);

		normal = (normal_tex.Sample(mysampler, input.uv).xyz) * 2 - float3(1, 1, 1);	
		normal = mul(transpose(float3x3(T, B, N)),  normal);
	}
	else
		normal = mul(input.normal_transform, float4(input.normal, 0)).xyz;
	normal = normalize(normal);

	//return pow(float4(0.5f * (normal + float3(1, 1, 1)), 1), 2.2);
	//return specular_tex.Sample(mysampler, input.uv);
	//return specular_exponent_tex.Sample(mysampler, input.uv);
	float shininess_exponenet = max(0.001f, specular_exponent_factor * specular_exponent_tex.Sample(mysampler, input.uv).r);
	float3 color = diffuse_tex.Sample(mysampler, input.uv).rgb;

	//
	//return float4(color, 1);	
	//return float4(color, 1);

	
	float3 specular_color = specular_tex.Sample(mysampler, input.uv).rgb;

	float3 to_camera = normalize(camera_p - input.world_p);

	float3 result = 0;
	
	{
		float3 to_light = -normalize(float3(-1, 0, -1));
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
	result += 0.2f * color;
#if 1
	float3 light_pos[2] = {
			player_p,
			float3(0, 2, 4)
	};
	float3 light_color[2] = {
		float3(0.8, 0.2, 0.3),
		float3(0.1, 0.9, 0.4)
	};
	for (int i = 0; i < 2; i++) {
		float3 to_light = normalize(light_pos[i] - input.world_p);
		float diffuse = max(0, dot(to_light, normal));
		
		float3 specular = specular_color * pow(max(0, dot(reflect(-to_light, normal), to_camera)), shininess_exponenet);
		//specular = 0;
		//specular = 0;	
		//diffuse = 0;
		//specular *= specular;
		result += light_color[i] * (diffuse_factor*color * diffuse  + specular_factor*specular) * 1.f / length(light_pos[i] - input.world_p) ;
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