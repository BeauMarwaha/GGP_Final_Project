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

	float4 totalColor = float4(0, 0, 0, 0);
	uint sampleCount = 0;

	for (int y = -blurAmount; y <= blurAmount; y++)
	{
		for (int x = -blurAmount; x <= blurAmount; x++)
		{
			// Adjust uv for blur
			float2 uv = input.uv + float2(x * pixelWidth, y * pixelHeight);
			// Retrieve the color of the current pixel from extracted render target texture
			totalColor += BrightPixels.Sample(Sampler, uv);

			sampleCount++;
		}
	}

	totalColor = totalColor / sampleCount;

	return totalColor + original;
}