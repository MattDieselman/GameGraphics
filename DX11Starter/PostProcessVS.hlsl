
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
};

// Data going out of the vertex shader (and eventually input to the PS)
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD;
};

VertexToPixel main( VertexShaderInput input )
{
	// Set up output
	VertexToPixel output;

	output.position = float4(input.position, 1);
	output.uv = input.uv;

	//// Calculate the UV (0,0) to (2,2) via the ID
	//output.uv = float2(
	//	(id << 2) & 2, // id % 2 * 2
	//	id & 2);

	//// Adjust the position based on the UV
	//output.position = float4(output.uv, 0, 1);
	//output.position.x = output.position.x * 2 - 1;
	//output.position.y = output.position.y * -2 + 1;

	return output;
}