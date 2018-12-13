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
	// Retrieve the color of the current pixel after normal rendering
	float4 original = Pixels.Sample(Sampler, input.uv);

	// Equation for determining how bright the pixel is
	float brightness = original.r*.2126 + original.g*.7152 + original.b*.0722;

	return original;	
}