#pragma once
#include "Player.h"

// For the DirectX Math library
using namespace DirectX;

Player::Player(Mesh* m, Material* mat, int type, Emitter * E_M_I_T) :
	Entity(m, mat, type)
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
	coolDown = 0.5f;
	lastShot = 0.0f;

	// Set emitter
	exhaustEmitter = E_M_I_T;

	// Set emitter values
	//exhaustEmitter->SetMaxParticles(100);
	exhaustEmitter->SetParticlesPerSecod(100);
	exhaustEmitter->SetLifetime(5);
	exhaustEmitter->SetStartSize(0.1f);
	exhaustEmitter->SetEndSize(5.0f);
	exhaustEmitter->SetStartColor(XMFLOAT4(1, 0.1f, 0.1f, 0.2f));
	exhaustEmitter->SetEndColor(XMFLOAT4(1, 0.6f, 0.1f, 0.0f));
	exhaustEmitter->SetEmitterVelocity(XMFLOAT3(0, 0, -1));
	exhaustEmitter->SetEmitterPosition(position);
	exhaustEmitter->SetEmitterAcceleration(XMFLOAT3(0, 0, 0));
}


Player::~Player()
{

}

void Player::Update(float deltaTime, float totalTime)
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		speed += 3.0f * deltaTime;

		if (speed >= 25.0f)
		{
			speed = 25.0f;
		}
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		speed -= 3.0f * deltaTime;

		if (speed <= -25.0f)
		{
			speed = -25.0f;
		}
	}
	else 
	{
		speed = 0.0f;
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		RotateBy(XMFLOAT3(0, -1 * abs(2.0F * deltaTime), 0));
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		RotateBy(XMFLOAT3(0, abs(2.0F * deltaTime), 0));
	}

	MoveForward(XMFLOAT3(0, 0, speed * deltaTime), deltaTime);

	// Shoot a bullet if the user hits space bar
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		Shoot(totalTime);
	}

	// Update the direction of the player based on rotation
	XMFLOAT3 orig = XMFLOAT3(0, 0, 1);
	XMStoreFloat3(&direction, XMVector3Rotate(XMLoadFloat3(&orig), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation))));

	Entity::Update(deltaTime, totalTime);
	/*exhaustEmitter->Update(deltaTime);*/
}

void Player::SetEntityManager(EntityManager * entityManager)
{
	this->entityManager = entityManager;
}

void Player::Draw(ID3D11DeviceContext * context, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
{
	Entity::Draw(context, viewMatrix, projectionMatrix);

	// Drawing particle systems
	//exhaustEmitter->Draw(context, viewMatrix, projectionMatrix); // Draw the emitter
}

void Player::Shoot(float totalTime)
{
	if (canShoot)
	{
		// Shoot
		entityManager->CreateEntity("Bullet_" + std::to_string(numBullets), "Bullet_Mesh", "Bullet_Material", EntityType::Bullet);

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
