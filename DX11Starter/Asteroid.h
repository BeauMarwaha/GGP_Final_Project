#pragma once
#include "Entity.h"

// --------------------------------------------------------
// An Asteroid class that represents a singular asteroid object
// Inherits from the Entity class
// --------------------------------------------------------
class Asteroid :
	public Entity
{
public:
	Asteroid(Mesh* m, Material* mat, int type);
	~Asteroid();

	// Update the asteroid
	void Update(float deltaTime, float totalTime);
};

