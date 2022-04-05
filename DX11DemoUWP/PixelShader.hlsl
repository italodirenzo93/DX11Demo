struct ps_in
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D shaderTexture;
SamplerState sampleType;

float4 main(ps_in input) : SV_TARGET
{
	return shaderTexture.Sample(sampleType, input.tex);
}
