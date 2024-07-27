#pragma pack_matrix(row_major)

cbuffer constants: register(b0)
{
    float4x4 mvp;
};

struct vs_in {
	float3 position: POSITION;
	float3 color: COLOR;
};

struct vs_out {
	float4 position: SV_POSITION;
	float3 color: COLOR;
};

vs_out vs_main(vs_in input)
{
	vs_out result;

    result.position = mul(mvp, float4(input.position, 1));
    result.color = input.color;

    return result;
}

float4 ps_main(vs_out input): SV_TARGET0
{
    return float4(input.color, 1);
}
