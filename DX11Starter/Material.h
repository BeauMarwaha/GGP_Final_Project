#pragma once

#include <DirectXMath.h>
#include "SimpleShader.h"
#include "WICTextureLoader.h"

class Material
{
public:
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* shaderResourceViewBaseColor, ID3D11ShaderResourceView* shaderResourceViewNormal, ID3D11SamplerState* samplerState); // Constructor
	~Material(); // Destructor

	// GET methods
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetShaderResourceViewBaseColor();
	ID3D11ShaderResourceView* GetShaderResourceViewNormal();
	ID3D11SamplerState* GetSamplerState();

private:
	// Wrappers for DirectX shaders to provide simplified shader functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	// The Shader Resource view for this material's base color texture (Required)
	ID3D11ShaderResourceView* shaderResourceViewBaseColor;

	// The Shader Resource view for this material's base color texture (Not Required)
	ID3D11ShaderResourceView* shaderResourceViewNormal;

	// The Sampler State for this material's texture
	ID3D11SamplerState* samplerState;
};

