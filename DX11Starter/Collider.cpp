#include "Collider.h"



Collider::Collider()
{
	enabled = true;
	radius = -1;
}

Collider::~Collider()
{
}

float Collider::GetRadius()
{
	return radius;
}

bool Collider::GetEnabled()
{
	return enabled;
}

void Collider::SetRadius(float value)
{
	radius = value;
}

void Collider::SetEnabled(bool value)
{
	enabled = value;
}
