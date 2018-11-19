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
	// NEEDS TO BE MOVED IF WORKS
	ID3D11RasterizerState * rasState = NULL;
	ID3D11BlendState * blendState = NULL;
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT blendMask = 0xffffffff;

	// Current Game State
	GameState state;

	// Current Game Scene
	SceneState currentScene;

	// Needed for sampling options (like filter and address modes)
	ID3D11SamplerState* sampler;

	// Sky stuff
	Mesh* skyMesh; // separate mesh to track specifically for skybox and not entities
	SimpleVertexShader* skyVS;
	SimplePixelShader* skyPS;
	ID3D11ShaderResourceView* skySRV;
	ID3D11RasterizerState* skyRastState;
	ID3D11DepthStencilState* skyDepthState;

	// look at me, I am the E_M_I_T now
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;

	// Initialization helper methods
	void CreateLights();
	void CreateEntities();
	void CreateSky();

	// Initialization Debug helper methods
	void CreateDebugLights();
	void CreateDebugEntities();

	// Update methods unique to game states
	void GameUpdate(float deltaTime, float totalTime);
	void DebugUpdate(float deltaTime, float totalTime);

	// Draw method unique to Skybox
	void DrawSky();

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

