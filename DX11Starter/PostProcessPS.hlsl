
cbuffer Data : register(b0)
{
	float pixelWidth;
	float pixelHeight;
	int blurAmountX;
	int blurAmountY;
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

	for (int y = -blurAmountY; y <= blurAmountY; y++)
	{
		// Get the uv for the current sample
		float2 uv = input.uv + float2(pixelWidth, y * pixelHeight);

		// Add to the total color
		totalColor += Pixels.Sample(Sampler, uv);
		numSamples++;
	}

	for (int x = -blurAmountX; x <= blurAmountX; x++)
	{
		// Get the uv for the current sample
		float2 uv = input.uv + float2(x * pixelWidth, pixelHeight);

		// Add to the total color
		totalColor += Pixels.Sample(Sampler, uv);
		numSamples++;
	}

	return totalColor / numSamples;
}