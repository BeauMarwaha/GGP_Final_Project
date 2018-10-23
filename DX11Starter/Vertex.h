#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 Position;	// The position of the vertex
	DirectX::XMFLOAT3 Normal;	// The Normal of the vertex (used in lighting)
	DirectX::XMFLOAT3 Tangent;	// The Tangent of the vertex (also used in lighting)
	DirectX::XMFLOAT2 UV;		// UV Coordinate of the vertex (used in texturing)
};