#pragma once
#include "Entity.h"
#include "EntityManager.h"

// --------------------------------------------------------
// A Player class that represents the player ship object
// Inherits from the Entity class
// --------------------------------------------------------
class Player :
	public Entity
{
public:
	Player(Mesh* m, Material* mat);
	~Player();

	// Update the player
	void Update(float deltaTime, float totalTime);

	// Set property for entity manager
	void SetEntityManager(EntityManager* entityManager);

private:
	// Shoot a bullet
	void Shoot(float totalTime);

	// Reference count for number of bullets
	int numBullets;

	// Reference to the entity manager in order to spawn bullets
	EntityManager* entityManager;

	// Shooting variables that ensure shooting bullets works on a timer
	bool canShoot;
	float coolDown;
	float lastShot;
};
