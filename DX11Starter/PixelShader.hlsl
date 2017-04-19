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
	float3 worldPos		: POSITION;
};

struct DirectionalLight
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 direction;
};

struct PointLight
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 location;
};

cbuffer data : register(b0)
{
	DirectionalLight dirLight;

	PointLight pointLight;

	float3 cameraPos;
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
	// Re-normalize interpolated normals
	input.normal = normalize(input.normal);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	float4 surfaceColor = diffuseTexture.Sample(sampState,input.uv);

	// Directional light calculations
	float3 dirToDL = normalize(-dirLight.direction);
	float dLightAmount = saturate(dot(input.normal, dirToDL));
	float4 dLightTotal = (dirLight.diffuseColor * dLightAmount) + dirLight.ambientColor;
	
	/*float3 dirTo2 = normalize(-light2.Direction);
	float3 light2Amount = saturate(dot(input.normal, dirTo2));
	float4 light2Total = (light2.diffuseColor * (float4(light2Amount, 1))) + light2.ambientColor;*/

	// Point light calculations
	float3 dirToPL = normalize(pointLight.location - input.worldPos);
	float pLightAmount = saturate(dot(input.normal, dirToPL));
	float4 pLightTotal = (pointLight.diffuseColor * pLightAmount) + pointLight.ambientColor;

	// Calculate specular amount for Point
	float3 dirToCamera = normalize(cameraPos - input.worldPos);
	float3 reflection = reflect(-dirToPL, input.normal);
	float specular = pow(saturate(dot(reflection, dirToCamera)), 64);

	// Final lighting calculations
	return (dLightTotal * surfaceColor) + (pLightTotal * surfaceColor) + specular.rrrr;
	//return (pLightTotal * surfaceColor) + specular.rrrr;
}