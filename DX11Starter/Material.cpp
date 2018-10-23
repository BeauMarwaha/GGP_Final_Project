#include "Material.h"

// For the DirectX Math library
using namespace DirectX;

Material::Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* shaderResourceViewBaseColor, ID3D11ShaderResourceView* shaderResourceViewNormal, ID3D11SamplerState* samplerState)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->shaderResourceViewBaseColor = shaderResourceViewBaseColor;
	this->shaderResourceViewNormal = shaderResourceViewNormal;
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

ID3D11ShaderResourceView * Material::GetShaderResourceViewBaseColor()
{
	return shaderResourceViewBaseColor;
}

ID3D11ShaderResourceView * Material::GetShaderResourceViewNormal()
{
	return shaderResourceViewNormal;
}

ID3D11SamplerState * Material::GetSamplerState()
{
	return samplerState;
}
