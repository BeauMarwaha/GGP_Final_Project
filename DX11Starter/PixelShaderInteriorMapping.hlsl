
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
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION; // The world position of this PIXEL
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

// Constant Buffer to hold interior mapping information
cbuffer interiorMappingData : register(b1)
{
	float3 CameraPosition; // Needed for specular (reflection) calculation
	float Offices;
	int NumCubeMaps;
	int RandSeed;
};

// Texture related global variables
TextureCubeArray	textureBaseColor	: register(t0);
TextureCube			SkyCube				: register(t1);
SamplerState		samplerState		: register(s0);

// Pseudo-random number generator from:
// https://gist.github.com/keijiro/ee7bc388272548396870
float rand(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

int RandomArrayIndex(float2 seed, int arrayLength)
{
	return (int)(rand(trunc(seed)) * arrayLength);
}

float3 RandomCubeRotation(float3 direction, float2 seed)
{
	// Rotate "randomly" either 0, 90, 180 or 270 degrees
	const float halfpi = 1.57079632679f;
	float rotate = trunc(rand(trunc(seed)) * 4) * halfpi;

	// Get sin and cos in one function call
	float s, c;
	sincos(rotate, s, c);

	// Build a Y-rotation matrix and rotate
	float3x3 rotMat = float3x3(
		c, 0, s,
		0, 1, 0,
		-s, 0, c);
	return mul(direction, rotMat);
}

// Use tangent space calculations?
//#define TANGENT_SPACE

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


	// Fix for poor normals: re-normalizing interpolated normals
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	// Create the TBN matrix which allows us to go from TANGENT space to WORLD space (or vice versa if we transpose)
	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);

	// View vector (in world space!)
	float3 view = normalize(input.worldPos - CameraPosition);

	// Reflection vector for window/sky reflection (handled here in world space)
	float3 refl = reflect(view, input.normal);
	float4 reflColor = SkyCube.Sample(samplerState, refl);

	// Super fake fresnel term to handle reflection amount (again, world space!)
	float fres = 1 - saturate(-dot(input.normal, view) * 0.75f);

	// Increase number of rooms by repeating the 0-1 uv space
	float2 uvScaled = input.uv * Offices;
	float2 uvFractional = frac(uvScaled);

	// Calculate a 3D position (from the UV coord) that
	// is essentially a ray's entry point into the box
	// we're trying to fake (this is TANGENT SPACE)
	float3 uvPosition = float3(uvFractional * 2 - 1, 1);

#ifdef TANGENT_SPACE
	// Change view to tangent space
	view = mul(view, transpose(TBN));
#else // WORLD SPACE!
	// Convert uv position from tangent to world space
	uvPosition = mul(uvPosition, TBN);
#endif

	// Simplified ray/box intersection test.  We're assuming that
	// the starting point of the ray is definitely inside the box!
	float3 viewInv = 1.0f / view;						// Invert the view ray
	float3 dist = abs(viewInv) - uvPosition * viewInv;	// Distance the ray travels
	float distMin = min(dist.x, min(dist.y, dist.z));	// Which plane is hit first?

	// Move along the view ray to the point we actually "see" inside the box
	float3 sampleDirection = uvPosition + distMin * view;

	// Randomly rotate the sample direction either 0, 90, 180 or 270 degrees
	float3 sampleDirRotated = RandomCubeRotation(sampleDirection, uvScaled);

	// Randomly pick a cube map
	int cubeIndex = RandomArrayIndex(uvScaled + RandSeed, NumCubeMaps);

	// Sample the interior cube map and interpolate between that and the reflection 
	float4 interiorColor = textureBaseColor.Sample(samplerState, float4(sampleDirRotated, cubeIndex));
	float4 windowColor = lerp(interiorColor, reflColor, fres);

	// Determine if we're on the "window frame"
	float2 frameSize = float2(0.45f, 0.45f);
	float2 distFromCenter = abs(float2(0.5f, 0.5f) - uvFractional);
	float frame = (float)(!any(distFromCenter > frameSize));

	// Return a color based on the window frame check
	// Note: frame will either be 0.0 or 1.0
	return windowColor * float4(frame.xxx, 1);
}