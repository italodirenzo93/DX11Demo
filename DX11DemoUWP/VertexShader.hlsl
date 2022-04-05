cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	matrix matWorldViewProj;
}

struct vs_in
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct ps_in
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

ps_in main(vs_in input)
{
	ps_in output;

	output.position = mul(matWorldViewProj, float4(input.position, 1.0f));
	output.color = float4(input.color, 1.0f);

	return output;
}
