
#include "Asteroid.h"

// For the DirectX Math library
using namespace DirectX;

Asteroid::Asteroid(Mesh* m, Material* mat, int type):
	Entity(m , mat, type)
{
	maxSpeed = 1;

	// Set a random position
	float x = 0;
	float z = 0;
	float minDist = 20;
	float scale = 20;
	do
	{
		x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
		x *= scale;
		z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
		z *= scale;
	} while ((x < minDist && x > -minDist) && (z < minDist && z > -minDist));

	position = XMFLOAT3(x, 0, z);

	// Set a random direction and drift speed that become the velocity
	direction = XMFLOAT3(rand() % 10 - 5, 0, rand() % 10 - 5);
	// Make sure to normalize the direction
	XMVECTOR tempDir = XMVector3Normalize(XMLoadFloat3(&direction));

	XMStoreFloat3(&direction, tempDir);

	XMStoreFloat3(&velocity, tempDir * (float)(rand() % (int)maxSpeed + 1));

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
