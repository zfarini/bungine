cbuffer constants: register(b0)
{
	row_major float4x4 view;
	row_major float4x4 projection;
	row_major float4x4 model;
};

struct vs_in {
    float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct vs_out {
    float4 position_clip : SV_POSITION;

};

vs_out vs_main(vs_in input)
{
	vs_out output;

	float4 world_p = mul(model, float4(input.position, 1));
	output.position_clip = mul(projection, mul(view, world_p));
	return output;
}

float4 ps_main(vs_out input) : SV_TARGET
{
	
	return float4(1, 0, 0, 1);
}