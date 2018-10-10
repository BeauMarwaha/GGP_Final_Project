#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A struct defining the properties of a directional light
// --------------------------------------------------------
struct DirectionalLight
{
	DirectX::XMFLOAT4 AmbientColor;	// The ambient color of the light
	DirectX::XMFLOAT4 DiffuseColor;	// The diffuse color of the light
	DirectX::XMFLOAT3 Direction;	// The direction the light is pointing
	float Padding;					// Manual Padding to ensure members don't cross 16-byte boundaries in memory
};