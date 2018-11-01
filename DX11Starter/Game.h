#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "EntityManager.h"
#include "MenuManager.h"
#include <DirectXMath.h>
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <vector>

enum class GameState
{
	Debug = 1,
	Game = 2
};
enum class SceneState {
	Main = 1,
	Game = 2,
	GameOver = 3
};

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
	// Current Game State
	GameState state;

	// Current Game Scene
	SceneState currentScene;

	// Initialization helper methods
	void CreateLights();
	void CreateEntities();

	// Initialization Debug helper methods
	void CreateDebugLights();
	void CreateDebugEntities();

	// Update methods unique to game states
	void GameUpdate(float deltaTime, float totalTime);
	void DebugUpdate(float deltaTime, float totalTime);

	// Menu Font
	DirectX::SpriteFont * font;

	// SpriteBatch for rendering menus
	DirectX::SpriteBatch * spriteBatch;

	// FPS camera
	Camera* camera;

	// Whether the debug camera is enabled
	bool debugCameraEnabled;

	// Entity Manager
	EntityManager* entityManager;

	// Menu Manager
	MenuManager * menuManager;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	// Indicates whether the left mouse button is pressed
	bool mouseDown;

	// Directional Lights
	DirectionalLight lights[4];
};

