#pragma once

#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"

// --------------------------------------------------------
// A Entity class that represents a singular game object
// --------------------------------------------------------
class Entity
{
public:
	Entity(Mesh* mesh, Material* material); // Constructor
	Entity(Entity const& other); // Copy Constructor
	Entity& operator=(Entity const& other); // Copy Assignment Operator
	~Entity(); // Destructor

	// Updates the game object
	void Update(float deltaTime, float totalTime);

	// GET methods
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();
	Mesh* GetMesh();

	// SET methods
	void SetWorldMatrix(DirectX::XMFLOAT4X4 worldMatrix);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetScale(DirectX::XMFLOAT3 scale);
	void SetMesh(Mesh* mesh);

	// Entity Transform Methods
	void Move(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 velocity);
	void MoveForward(DirectX::XMFLOAT3 velocity);

	// Helper methods
	DirectX::XMFLOAT4X4 GetIdentityMatrix();
	void Draw(ID3D11DeviceContext* context, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);
	void PrepareMaterial(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);

private:
	// World Matrix representing the entity’s current position, rotation, and scale
	DirectX::XMFLOAT4X4 worldMatrix;

	// Position, Rotation, Scale Vectors
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	// Entity Mesh
	Mesh* mesh;

	// Entity Material
	Material* material;
};

