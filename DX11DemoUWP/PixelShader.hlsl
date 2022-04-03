struct ps_in
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 main(ps_in input) : SV_TARGET
{
	return input.color;
}
