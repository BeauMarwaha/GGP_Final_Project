#pragma once
class Collider
{
public:
	Collider();
	~Collider();

	// Getters
	float GetRadius();
	bool GetEnabled();
	
	// Setters
	void SetRadius(float value);
	void SetEnabled(bool value);
private:
	// The radius of the collider
	float radius;

	// Whether or not the collider is enabled
	bool enabled;
};

