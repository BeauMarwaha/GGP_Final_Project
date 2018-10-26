#pragma once
class Collider
{
public:
	Collider();
	~Collider();

	// Getters
	float GetRadius();
	
	// Setters
	void SetRadius(float value);
private:
	float radius;

	bool enabled;
};

