#pragma once

#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Collider.h"

// --------------------------------------------------------
// A Entity class that represents a singular game object
// --------------------------------------------------------
class Entity
{
public:
	Entity(Mesh* mesh, Material* material, int type); // Constructor
	Entity(Entity const& other); // Copy Constructor
	Entity& operator=(Entity const& other); // Copy Assignment Operator
	bool operator==(Entity const& other);
	~Entity(); // Destructor

	// Updates the game object
	void virtual Update(float deltaTime, float totalTime);

	// GET methods
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetVelocity();
	DirectX::XMFLOAT3 GetDirection();
	float GetMaxSpeed();
	int GetType();
	Collider GetCollider();
	Mesh* GetMesh();

	// SET methods
	void SetWorldMatrix(DirectX::XMFLOAT4X4 worldMatrix);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetScale(DirectX::XMFLOAT3 scale);
	void SetUniformScale(float scale);
	void SetDirection(DirectX::XMFLOAT3 direction);
	void SetMesh(Mesh* mesh);

	// Entity Transform Methods
	void Move(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 velocity);
	void MoveForward(DirectX::XMFLOAT3 velocity, float dTime);
	void RotateBy(DirectX::XMFLOAT3 deltaRotation);

	// Helper methods
	DirectX::XMFLOAT4X4 GetIdentityMatrix();
	void Draw(ID3D11DeviceContext* context, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);
	void PrepareMaterial(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);

	float speed;
	DirectX::XMVECTOR moveDir;

private:

	//I exist to be a change for a small merge to fix master.

protected:

	// World Matrix representing the entity’s current position, rotation, and scale
	DirectX::XMFLOAT4X4 worldMatrix;

	// Position, Rotation, Scale Vectors
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	// Velocity, direction, and Max Speed for movement of entities
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 direction;
	float maxSpeed;

	// Boolean for if the world matrix needs to be recalculated
	bool isWorldDirty;

	// Entity Mesh
	Mesh* mesh;

	// Entity Material
	Material* material;

	// Entity Collider
	Collider collider;

	// Entity Type
	int type;
};

