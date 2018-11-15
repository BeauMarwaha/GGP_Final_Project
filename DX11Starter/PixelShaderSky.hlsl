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
	float3 direction	: TEXCOORD;
};

// Texture related global variables
TextureCube SkyTextureBase	: register(t0);
SamplerState samplerState	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	// Sample the final pixel color from the passed in texture cube and sample direction
	return SkyTextureBase.Sample(samplerState, input.direction);
}