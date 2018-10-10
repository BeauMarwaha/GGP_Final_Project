#pragma once

#include <DirectXMath.h>
#include "SimpleShader.h"
#include "WICTextureLoader.h"

class Material
{
public:
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* shaderResourceView, ID3D11SamplerState* samplerState); // Constructor
	~Material(); // Destructor

	// GET methods
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11SamplerState* GetSamplerState();

private:
	// Wrappers for DirectX shaders to provide simplified shader functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	// The Shader Resource view for this material's texture
	ID3D11ShaderResourceView* shaderResourceView;

	// The Sampler State for this material's texture
	ID3D11SamplerState* samplerState;
};

