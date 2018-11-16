#include "Camera.h"

// For the DirectX Math library
using namespace DirectX;

Camera::Camera(unsigned int width, unsigned int height)
{
	// Initialize Variables
	XMStoreFloat4x4(&viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&projectionMatrix, XMMatrixIdentity());
	position = XMFLOAT3(0, 0, -5);
	xRotation = 0;
	yRotation = 0;
	speed = 5;

	// Set the initial projection matrix
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * PI,	// Field of View Angle
		(float)width / height,	// Aspect ratio
		0.1f,				  	// Near clip plane distance
		100.0f);			  	// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}


Camera::~Camera()
{
}

void Camera::Update(float deltaTime, float totalTime, Entity* player, bool debugCameraEnabled)
{
	// Calculate the camera's view matrix
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(xRotation, yRotation, 0);
	XMFLOAT4 forward = XMFLOAT4(0, 0, 1, 0);
	XMVECTOR newEyeDirection = XMVector4Transform(XMLoadFloat4(&forward), rotation);
	XMFLOAT4 up = XMFLOAT4(0, 1, 0, 0);
	XMVECTOR newUpDirection = XMVector4Transform(XMLoadFloat4(&up), rotation);
	XMMATRIX newViewMatrix = XMMatrixLookToLH(XMLoadFloat3(&position), newEyeDirection, newUpDirection);

	// Update the camera's view matrix
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(newViewMatrix)); // Transpose for HLSL

	// Move the camera
	Move(deltaTime, player, debugCameraEnabled);
}

void Camera::Move(float deltaTime, Entity* player, bool debugCameraEnabled)
{
	// Check if the debug camera is enabled, if so use manual player control
	if (debugCameraEnabled)
	{
		// Set initial velocity to 0
		XMFLOAT3 velocity = XMFLOAT3(0, 0, 0);

		// Check for camera input
		if (GetAsyncKeyState('I') & 0x8000)
		{
			velocity.z += speed * deltaTime;
		}

		if (GetAsyncKeyState('K') & 0x8000)
		{
			velocity.z -= speed * deltaTime;
		}

		if (GetAsyncKeyState('J') & 0x8000)
		{
			velocity.x -= speed * deltaTime;
		}

		if (GetAsyncKeyState('L') & 0x8000)
		{
			velocity.x += speed * deltaTime;
		}

		if (GetAsyncKeyState('U') & 0x8000)
		{
			velocity.y += speed * deltaTime;
		}

		if (GetAsyncKeyState('O') & 0x8000)
		{
			velocity.y -= speed * deltaTime;
		}

		// Update camera position based on input
		XMVECTOR initialPos = XMLoadFloat3(&position);
		XMVECTOR movement = XMVector3Rotate(XMLoadFloat3(&velocity), XMQuaternionRotationRollPitchYaw(xRotation, yRotation, 0));
		XMStoreFloat3(&position, initialPos + movement);

		return;
	}

	// If the debug camera is not enabled update the camera position based on player location
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(xRotation, yRotation, 0);
	XMFLOAT4 forward = XMFLOAT4(0, 0, 1, 0);
	XMVECTOR newForward = XMVector4Transform(XMLoadFloat4(&forward), rotation);
	XMFLOAT4 playerForward = XMFLOAT4(0, 0, 1, 0);
	XMStoreFloat4(&playerForward, newForward);

	// Have the camera sit slightly above and behind the player
	position.x = player->GetPosition().x - (playerForward.x * 10);
	position.y = player->GetPosition().y + 3.0f;	
	position.z = player->GetPosition().z - (playerForward.z * 10);

	// Have the rotation follow the player
	xRotation = player->GetRotation().x + .3f;
	yRotation = fmod(player->GetRotation().y, PI * 2);
}

void Camera::Rotate(float deltaX, float deltaY)
{
	xRotation += deltaX;
	yRotation += deltaY;

	// Clamp the xRotation
	if (xRotation > PI)
		xRotation = PI;
	if (xRotation < -PI)
		xRotation = -PI;

	// Loop the yRotation
	if (yRotation > PI*2)
		yRotation = 0;
	if (yRotation < -PI*2)
		yRotation = 0;
}

void Camera::ResizeWindow(unsigned int width, unsigned int height)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * PI,	// Field of View Angle
		(float)width / height,	// Aspect ratio
		0.1f,				  	// Near clip plane distance
		100.0f);			  	// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}
