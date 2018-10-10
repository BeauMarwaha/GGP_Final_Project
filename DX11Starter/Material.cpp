#include "Material.h"

// For the DirectX Math library
using namespace DirectX;

Material::Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* shaderResourceView, ID3D11SamplerState* samplerState)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->shaderResourceView = shaderResourceView;
	this->samplerState = samplerState;
}

Material::~Material()
{
}

SimpleVertexShader * Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Material::GetShaderResourceView()
{
	return shaderResourceView;
}

ID3D11SamplerState * Material::GetSamplerState()
{
	return samplerState;
}
