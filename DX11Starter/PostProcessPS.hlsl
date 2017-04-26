
cbuffer Data : register(b0)
{
	float pixelWidth;
	float pixelHeight;
	int blurAmount;
}

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
	// Total color for the overall average
	float4 totalColor = float4(0,0,0,0);
	uint numSamples = 0;

	for (int y = -blurAmount; y <= blurAmount; y++)
	{
		for (int x = -blurAmount; x <= blurAmount; x++)
		{
			// Get the uv for the current sample
			float2 uv = input.uv + float2(x * pixelWidth, y * pixelHeight);

			// Add to the total color
			totalColor += Pixels.Sample(Sampler, uv);
			numSamples++;
		}
	}

	return totalColor / numSamples;
}