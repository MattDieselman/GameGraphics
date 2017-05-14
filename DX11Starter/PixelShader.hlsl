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
	float3 worldPos		: POSITION;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float2 uv			: TEXCOORD;
	float4 spot1ShadowPos : POSITION1;
	float4 spot2ShadowPos : POSITION2;
	float4 dirShadowPos	: POSITION3;
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

struct SpotLight
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 location;
	float3 direction;
	float angle;
};

cbuffer data : register(b0)
{
	DirectionalLight dirLight;

	PointLight pointLight;

	SpotLight spotLight;
	SpotLight spotLight2;

	float3 cameraPos;
};

Texture2D diffuseTexture	: register(t0);
Texture2D normalMap			: register(t1);
Texture2D spot1ShadowMap	: register(t2);
Texture2D spot2ShadowMap	: register(t3);
Texture2D dirShadowMap		: register(t4);
SamplerState sampState		: register(s0);
SamplerComparisonState ShadowSampler : register(s1);

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
	input.tangent = normalize(input.tangent);

	// Get the normal map sample
	float4 normalFromMap = normalMap.Sample(sampState, input.uv) * 2 - 1;

	// Calculate the TBN matrix to go from tangent space to world space
	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);
	input.normal = normalize(mul(normalFromMap, TBN));

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

	// Spot light calculations
	float3 dirToSL = normalize(spotLight.location - input.worldPos);
	float angleToObj = acos(dot(-dirToSL, spotLight.direction));
	uint objInRange = step(angleToObj, spotLight.angle); // If angleToObj is bigger return 0, else return 1
	float sLightAmount = saturate(dot(input.normal, dirToSL)) * objInRange;
	float4 sLightTotal = (spotLight.diffuseColor * sLightAmount) + spotLight.ambientColor;
	// Spot light 2 calculations
	float3 dirToSL2 = normalize(spotLight2.location - input.worldPos);
	float angleToObj2 = acos(dot(-dirToSL2, spotLight2.direction));
	uint objInRange2 = step(angleToObj2, spotLight2.angle); // If angleToObj is bigger return 0, else return 1
	float sLightAmount2 = saturate(dot(input.normal, dirToSL2)) * objInRange2;
	float4 sLightTotal2 = (spotLight2.diffuseColor * sLightAmount2) + spotLight2.ambientColor;

	// Sample the texture
	float4 surfaceColor = diffuseTexture.Sample(sampState, input.uv);

	// Shadow mapping calculations and sample
	// Spot 1
	float depthFromLight1 = input.spot1ShadowPos.z / input.spot1ShadowPos.w;
	float2 shadowUV1 = input.spot1ShadowPos.xy / input.spot1ShadowPos.w * 0.5f + 0.5f;
	shadowUV1.y = 1.0f - shadowUV1.y;
	float spot1ShadowAmount = spot1ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV1, depthFromLight1);
	// Spot 2
	float depthFromLight2 = input.spot2ShadowPos.z / input.spot2ShadowPos.w;
	float2 shadowUV2 = input.spot2ShadowPos.xy / input.spot2ShadowPos.w * 0.5f + 0.5f;
	shadowUV2.y = 1.0f - shadowUV2.y;
	float spot2ShadowAmount = spot2ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV2, depthFromLight2);
	// Dir
	float depthFromLight3 = input.dirShadowPos.z / input.dirShadowPos.w;
	float2 shadowUV3 = input.dirShadowPos.xy / input.dirShadowPos.w * 0.5f + 0.5f;
	shadowUV3.y = 1.0f - shadowUV3.y;
	float dirShadowAmount = dirShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV3, depthFromLight3);

	// Final lighting calculations
	//return shadowAmount;
	return (dLightTotal * surfaceColor * dirShadowAmount) + (pLightTotal * surfaceColor) + (sLightTotal * surfaceColor * spot1ShadowAmount) + (sLightTotal2 * surfaceColor * spot2ShadowAmount) + specular.rrrr;
	//return (dLightTotal * surfaceColor) + (pLightTotal * surfaceColor) + (sLightTotal * surfaceColor) + specular.rrrr;
	//return (sLightTotal * surfaceColor);
}