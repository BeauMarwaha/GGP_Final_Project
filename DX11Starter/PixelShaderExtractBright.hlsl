// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

// Textures and such
Texture2D Pixels		: register(t0);
SamplerState Sampler	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	// Retrieve the current color of the pixel after normal rendering
	float4 og = Pixels.Sample(Sampler, input.uv);
	return float4(1.0f, 1.0f, 1.0f, 1.0f);	
}