cbuffer constants: register(b0)
{
	row_major float4x4 view;
	row_major float4x4 projection;
};

struct vs_in {
    float3 position : POSITION;
};

struct vs_out {
    float4 position_clip : SV_POSITION;

};

vs_out vs_main(vs_in input)
{
	vs_out output;
	output.position_clip = mul(projection, mul(view, float4(input.position, 1)));
	return output;
}

float4 ps_main(vs_out input) : SV_TARGET
{
	
	return float4(1, 0, 0, 1);
}