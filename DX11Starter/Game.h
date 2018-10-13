#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "EntityManager.h"
#include <DirectXMath.h>
#include <vector>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);

private:
	// Initialization helper methods
	void CreateLights();
	void CreateEntities();

	// FPS camera
	Camera* camera;

	// Entity Manager
	EntityManager* entityManager;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	// Indicates whether the left mouse button is pressed
	bool mouseDown;

	// Directional Lights
	DirectionalLight lights[4];
};

