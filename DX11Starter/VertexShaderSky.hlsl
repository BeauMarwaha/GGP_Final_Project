// Constant Buffer
// - Allows us to define a buffer of individual variables 
//    which will (eventually) hold data from our C++ code
// - All non-pipeline variables that get their values from 
//    our C++ code must be defined inside a Constant Buffer
// - The name of the cbuffer itself is unimportant
cbuffer externalData : register(b0)
{
	matrix view;
	matrix projection;
};

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;     // XYZ position
	float2 uv			: TEXCOORD;     // Won't use any of these after position, but we need to match the vertex def
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 direction	: TEXCOORD;     // XYZ direction 
};

VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	// Adjust the view matrix to have no translation so that the sky is always centered
	matrix viewSky = view;
	viewSky._41 = 0;
	viewSky._42 = 0;
	viewSky._43 = 0;

	// Multiply position from the input by the proj and view matrix to get new pos for output
	matrix viewProj = mul(viewSky, projection);
	output.position = mul(float4(input.position, 1), viewProj);

	// Push the vertex out to the far clip plane (Z of 1)
	output.position.z = output.position.w; // Z gets divided by W automatically

	// Pass float3 position of the vertex into direction output to pixel shader
	output.direction = input.position;

	return output;
}