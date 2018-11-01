#pragma once
#include "Player.h"

// For the DirectX Math library
using namespace DirectX;

Player::Player(Mesh* m, Material* mat) :
	Entity(m, mat)
{
	maxSpeed = 5;

	numBullets = 0;

	// Start position at the origin
	position = XMFLOAT3(0, 0, 0);

	// Set the direction to face forward and the starting velocity to 0
	direction = XMFLOAT3(0, 0, 1);
	velocity = XMFLOAT3(0, 0, 0);

	// Initialize shooting vars
	canShoot = true;
	coolDown = 1.0f;
	lastShot = 0.0f;
}


Player::~Player()
{

}

void Player::Update(float deltaTime, float totalTime)
{

	if (GetAsyncKeyState('W') & 0x8000)
	{
		MoveForward(XMFLOAT3(0, 0, maxSpeed * deltaTime));
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		MoveForward(XMFLOAT3(0, 0, -maxSpeed * deltaTime));
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		RotateBy(XMFLOAT3(0, -maxSpeed * deltaTime, 0));
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		RotateBy(XMFLOAT3(0, maxSpeed * deltaTime, 0));
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		Shoot(totalTime);
	}

	// Update the direction of the player based on rotation
	XMFLOAT3 orig = XMFLOAT3(0, 0, 1);
	XMStoreFloat3(&direction, XMVector3Rotate(XMLoadFloat3(&orig), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation))));

	Entity::Update(deltaTime, totalTime);
}

void Player::SetEntityManager(EntityManager * entityManager)
{
	this->entityManager = entityManager;
}

void Player::Shoot(float totalTime)
{
	if (canShoot)
	{
		// Shoot
		entityManager->CreateEntity("Bullet"+numBullets, "Sphere_Mesh", "Snow_Material", EntityType::Bullet);

		numBullets++;

		// Reset cooldown timer
		canShoot = false;
		lastShot = totalTime;
	}
	else if (totalTime > coolDown + lastShot)
	{
 		canShoot = true;
	}
}
