#include "Asteroid.h"
#include <ctime> 

// For the DirectX Math library
using namespace DirectX;

Asteroid::Asteroid(Mesh* m, Material* mat):
	Entity(m , mat)
{
	srand((unsigned)time(0));

	// Set a random position
	position = XMFLOAT3(rand() % 10, rand() % 10, rand() % 10);

	// Set a random direction and drift speed that become the velocity
	direction = XMFLOAT3(rand() % 10, rand() % 10, rand() % 10);
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
	XMStoreFloat3(&position, XMLoadFloat3(&position) + XMLoadFloat3(&velocity));

	// Run base entity update
	Entity::Update(deltaTime, totalTime);
}
