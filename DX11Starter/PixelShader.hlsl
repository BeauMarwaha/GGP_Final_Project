
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
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
};

// Struct representing a directional light
struct DirectionalLight
{
	float4 ambientColor;	// The ambient color of the light
	float4 diffuseColor;	// The diffuse color of the light
	float3 direction;		// The direction the light is pointing
	float padding;			// Manual Padding to ensure members don't cross 16-byte boundaries in memory
};

// Constant Buffer to hold light information
cbuffer lightData : register(b0)
{
	DirectionalLight lights[4]; // The size of this array should match the number of lights getting passed in
};

// Texture related global variables
Texture2D textureBaseColor	: register(t0);
SamplerState samplerState	: register(s0);

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
	// Normalize the passed in normal 
	// - Previous transformations may have made it a non-unit vector
	input.normal = normalize(input.normal);

	// Calculate the lighting impact on final pixel color for each passed in light
	float4 lightColor = float4(0, 0, 0, 1);
	for (int i = 0; i < 4; i++)
	{
		// Calculate the normalized direction to the light
		float3 directionToTheLight = normalize(-lights[i].direction);

		// Calculate the light amount using the N dot L equation
		// - Use the dot(v1, v2) function with the surface’s normal and the direction to the light
		// - The normal should already be normalized from a previous step
		// - The dot product result can be negative, which will be problematic if we have multiple
		//   lights.Use the saturate() function to clamp the result between 0 and 1
		float lightAmount = saturate(dot(input.normal, directionToTheLight));

		// Add to the final surface color based on light amount, diffuse color and ambient color
		// - Scale the light’s diffuse color by the light amount
		// - Add the light’s ambient color
		lightColor += (lightAmount * lights[i].diffuseColor) + lights[i].ambientColor;
	}

	// Sample the base final pixel color from the passed in texture and uv cordinates
	float4 surfaceColor = textureBaseColor.Sample(samplerState, input.uv);

	// Return the final pixel color
	return surfaceColor * lightColor;
}