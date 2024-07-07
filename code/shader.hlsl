struct vs_in {
    float3 position : POSITION;
};

struct vs_out {
    float4 position_clip : SV_POSITION;

};

vs_out vs_main(vs_in input)
{
	vs_out output;
	output.position_clip = float4(input.position, 1);
	return output;
}

float4 ps_main(vs_out input) : SV_TARGET
{
	
	return float4(1, 0, 0, 1);
}