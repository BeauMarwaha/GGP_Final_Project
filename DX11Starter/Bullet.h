#pragma once
#include "Entity.h"

// --------------------------------------------------------
// A Bullet class that represents a singular bullet object
// Inherits from the Entity class
// Only ever Instantiated by the Player class
// --------------------------------------------------------
class Bullet :
	public Entity
{
public:
	Bullet(Mesh* m, Material* mat);
	~Bullet();

	// Update the bullet
	void Update(float deltaTime, float totalTime);
};

