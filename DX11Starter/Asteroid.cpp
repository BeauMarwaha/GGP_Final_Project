#include "Asteroid.h"

// For the DirectX Math library
using namespace DirectX;

Asteroid::Asteroid(Mesh* m, Material* mat):
	Entity(m , mat)
{
	maxSpeed = 1;

	// Set a random position
	position = XMFLOAT3(rand() % 10, 0, rand() % 10);

	// Set a random direction and drift speed that become the velocity
	direction = XMFLOAT3(rand() % 10, 0, rand() % 10);
	// Make sure to normalize the direction
	XMVECTOR tempDir = XMVector3Normalize(XMLoadFloat3(&direction));

	XMStoreFloat3(&direction, tempDir);

	XMStoreFloat3(&velocity, tempDir * (float)(rand() % maxSpeed + 1));

}


Asteroid::~Asteroid()
{

}

void Asteroid::Update(float deltaTime, float totalTime)
{
	XMStoreFloat3(&position, (XMLoadFloat3(&position) + XMLoadFloat3(&velocity)* deltaTime));
	// Set the world matrix to dirty so that it updates
	isWorldDirty = true;

	// Run base entity update
	Entity::Update(deltaTime, totalTime);
}
