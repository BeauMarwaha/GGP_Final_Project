#include "Bullet.h"

// For the DirectX Math library
using namespace DirectX;

Bullet::Bullet(Mesh* m, Material* mat, int type) :
	Entity(m, mat, type)
{
	maxSpeed = 25;

	// Set scale smaller
	scale = XMFLOAT3(.5f, .5f, .5f);

	// Position and Direction are set by EntityManager to match the player's at the time of firing
	
	// Make sure to normalize the direction
	XMVECTOR tempDir = XMVector3Normalize(XMLoadFloat3(&direction));
	XMStoreFloat3(&direction, tempDir);

	// Calculate the velocity of the bullet
	XMStoreFloat3(&velocity, tempDir * maxSpeed);
}


Bullet::~Bullet()
{

}

void Bullet::Update(float deltaTime, float totalTime)
{
	// Calculate the velocity of the bullet
	XMStoreFloat3(&velocity, XMLoadFloat3(&direction) * maxSpeed);

	XMStoreFloat3(&position, (XMLoadFloat3(&position) + XMLoadFloat3(&velocity)* deltaTime));
	// Set the world matrix to dirty so that it updates
	isWorldDirty = true;

	// Run base entity update
	Entity::Update(deltaTime, totalTime);
}
