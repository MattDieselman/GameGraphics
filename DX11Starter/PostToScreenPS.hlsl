// Input data from VS
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD;
};

// Textures and Samplers
Texture2D Pixels		: register(t0);
SamplerState Sampler	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	return Pixels.Sample(Sampler, input.uv);
}