cbuffer Data : register(b0)
{
	float pixelWidth;
	float pixelHeight;
	int blurAmount;
}

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

// Textures and such
Texture2D Pixels		: register(t0);
Texture2D BrightPixels	: register(t1);
SamplerState Sampler	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	// Retrieve the color of the current pixel after normal rendering
	float4 original = Pixels.Sample(Sampler, input.uv);

	// Retrieve the color of the current pixel from extracted render target texture
	float4 bright = BrightPixels.Sample(Sampler, input.uv);

	return original;
}