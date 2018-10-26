#include "Collider.h"



Collider::Collider()
{
	radius = -1;
}


Collider::~Collider()
{
}

float Collider::GetRadius()
{
	return radius;
}

void Collider::SetRadius(float value)
{
	radius = value;
}
