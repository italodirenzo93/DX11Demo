#include "common.hlsli"

Texture2D shaderTexture;
SamplerState sampleType;

float4 main(ps_in input) : SV_TARGET
{
	return shaderTexture.Sample(sampleType, input.tex);
}
