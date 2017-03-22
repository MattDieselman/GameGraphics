


// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD;        // RGBA color
	float3 normal		: NORMAL;
};
struct DirectionalLight {
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

cbuffer data : register(b0)
{
	DirectionalLight light;
	DirectionalLight light2;
};
Texture2D diffuseTexture : register(t0);
SamplerState sampState : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	float4 surfaceColor = diffuseTexture.Sample(sampState,input.uv);

	float3 dirTo = normalize(-light.Direction);
	float3 lightAmount = saturate(dot(input.normal, dirTo));
	float4 light1Tot = (light.DiffuseColor*(float4(lightAmount,1)))+light.AmbientColor;
	float3 dirTo2 = normalize(-light2.Direction);
	float3 light2Amount = saturate(dot(input.normal, dirTo2));
	float4 light2Tot = (light2.DiffuseColor*(float4(light2Amount, 1))) + light2.AmbientColor;
	return (light1Tot*surfaceColor) + (light2Tot *surfaceColor);
	//return float4(input.normal,1);
	//return light.DiffuseColor;
	//return float4(1, 0, 0, 1);
}