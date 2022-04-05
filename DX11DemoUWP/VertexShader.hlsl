cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	matrix matWorldViewProj;
}

struct vs_in
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct ps_in
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

ps_in main(vs_in input)
{
	ps_in output;

	output.position = mul(matWorldViewProj, float4(input.position, 1.0f));
	output.tex = input.tex;

	return output;
}
