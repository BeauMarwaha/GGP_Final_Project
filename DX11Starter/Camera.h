#pragma once

#include <DirectXMath.h>
#include <Windows.h>

class Camera
{
public:
	Camera(unsigned int width, unsigned int height); // Constructor
	~Camera(); // Destructor

	// Update method
	void Update(float deltaTime, float totalTime);

	// Helper methods
	void Move(float deltaTime);
	void Rotate(float deltaX, float deltaY);
	void ResizeWindow(unsigned int width, unsigned int height);

	// GET methods
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

private:
	// Matricies holding the camera's current view and projection matrix
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Fields for creating a 'look to' view matrix
	DirectX::XMFLOAT3 position;
	float xRotation;
	float yRotation;

	// Speed of the camera
	float speed;

	// Size of the window
	float screenWidth;
	float screenHeight;

	// Declare PI constant
	const float PI = 3.1415926535f;
};

