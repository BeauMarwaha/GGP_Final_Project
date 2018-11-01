#include "Game.h"
#include "Vertex.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	
	mouseDown = false;
	camera = new Camera(width, height);
	debugCameraEnabled = false;
	entityManager = new EntityManager();
	

	// Set the game state to the debug scene
	state = GameState::Debug;
	currentScene = SceneState::Main;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Delete the camera
	delete camera;

	// Delete the entity manager
	delete entityManager;

	// Delete the menu manager
	delete menuManager;

	// delete the font
	delete font;

	// Delete the sprite batch
	delete spriteBatch;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	font = new SpriteFont(device, L"resources/fonts/MenuFont.spritefont");
	menuManager = new MenuManager(font);
	// Initialize SpriteBatch
	spriteBatch = new SpriteBatch(context);

	// Helper methods for initialization based on game state
	switch (state)
	{
	case GameState::Debug:
		CreateDebugLights();
		CreateDebugEntities();
		break;
	case GameState::Game:
		CreateLights();
		CreateEntities();
		break;
	}

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Sets up all light sources
// --------------------------------------------------------
void Game::CreateLights()
{
	// Set up the directional light sources
	lights[0].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[0].DiffuseColor = XMFLOAT4(0, 0, 1, 1);
	lights[0].Direction = XMFLOAT3(1, -1, 0);
	lights[1].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[1].DiffuseColor = XMFLOAT4(0, 1, 0, 1);
	lights[1].Direction = XMFLOAT3(-1, 1, 0);
	lights[2].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[2].DiffuseColor = XMFLOAT4(1, 0, 0, 1);
	lights[2].Direction = XMFLOAT3(-1, -1, 0);
	lights[3].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[3].DiffuseColor = XMFLOAT4(1, 1, 1, 1);
	lights[3].Direction = XMFLOAT3(1, 1, 0);
}

// --------------------------------------------------------
// Sets up entities in the entity manager, this includes:
// - Creates basic materials to be used in engine, this includes:
//   - Loading shaders from compiled shader object (.cso) files using
//     my SimpleShader wrapper for DirectX shader manipulation
//     - SimpleShader provides helpful methods for sending
//       data to individual variables on the GPU
//   - Using DirectXTK to load a texture from an external file
//   - And creating a sampler description which specifies how the 
//     material should sample from the loaded texture
// - Loads the geometry we're going to draw from external files
// --------------------------------------------------------
void Game::CreateEntities()
{
	// Create the vertex shaders
	entityManager->CreateVertexShader("Normals_Vertex_Shader", device, context, L"VertexShaderNormals.cso");

	// Create the pixel shaders
	entityManager->CreatePixelShader("Normals_Pixel_Shader", device, context, L"PixelShaderNormals.cso");

	// Create the rock shader resource view
	entityManager->CreateShaderResourceView("Cliff_Texture", device, context, L"resources/textures/CliffLayered_bc.tif");
	entityManager->CreateShaderResourceView("Cliff_Normal_Texture", device, context, L"resources/textures/CliffLayered_normal.tif");

	// Define the anisotropic filtering sampler description
	D3D11_SAMPLER_DESC samplerDesc = {}; // Zero out all values initially
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the U axis
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the V axis
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the W axis
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; // Use Anisotropic filtering
	samplerDesc.MaxAnisotropy = 16; // Use x16 anisotropy
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // This value needs to be higher than 0 for mipmapping to work

	// Create the anisotropic filtering sampler state
	entityManager->CreateSamplerState("Anisotropic_Sampler", device, samplerDesc);

	// Create the rock material using the previously set up resources
	entityManager->CreateMaterialWithNormal("Cliff_Normal_Material", "Normals_Vertex_Shader", "Normals_Pixel_Shader", "Cliff_Texture", "Cliff_Normal_Texture", "Anisotropic_Sampler");
	
	// Load geometry
	entityManager->CreateMesh("Sphere_Mesh", device, "resources/models/sphere.obj");

	// Create entities using the previously set up resources
	entityManager->CreateEntity("Player", "Sphere_Mesh", "Cliff_Normal_Material");
}

// --------------------------------------------------------
// Sets up all light sources for the debug scene
// --------------------------------------------------------
void Game::CreateDebugLights()
{
	// Set up the directional light sources
	lights[0].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[0].DiffuseColor = XMFLOAT4(0, 0, 1, 1);
	lights[0].Direction = XMFLOAT3(1, -1, 0);
	lights[1].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[1].DiffuseColor = XMFLOAT4(0, 1, 0, 1);
	lights[1].Direction = XMFLOAT3(-1, 1, 0);
	lights[2].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[2].DiffuseColor = XMFLOAT4(1, 0, 0, 1);
	lights[2].Direction = XMFLOAT3(-1, -1, 0);
	lights[3].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[3].DiffuseColor = XMFLOAT4(1, 1, 1, 1);
	lights[3].Direction = XMFLOAT3(1, 1, 0);
}

// --------------------------------------------------------
// Sets up entities in the entity manager for the debig scene, this includes:
// - Creates basic materials to be used in engine, this includes:
//   - Loading shaders from compiled shader object (.cso) files using
//     my SimpleShader wrapper for DirectX shader manipulation
//     - SimpleShader provides helpful methods for sending
//       data to individual variables on the GPU
//   - Using DirectXTK to load a texture from an external file
//   - And creating a sampler description which specifies how the 
//     material should sample from the loaded texture
// - Loads the geometry we're going to draw from external files
// --------------------------------------------------------
void Game::CreateDebugEntities()
{
	// Create the vertex shaders
	entityManager->CreateVertexShader("Default_Vertex_Shader", device, context, L"VertexShader.cso");
	entityManager->CreateVertexShader("Normals_Vertex_Shader", device, context, L"VertexShaderNormals.cso");

	// Create the pixel shaders
	entityManager->CreatePixelShader("Default_Pixel_Shader", device, context, L"PixelShader.cso");
	entityManager->CreatePixelShader("Normals_Pixel_Shader", device, context, L"PixelShaderNormals.cso");

	// Create the rock shader resource view
	entityManager->CreateShaderResourceView("Gravel_Texture", device, context, L"resources/textures/GravelCobble_bc.jpg");
	entityManager->CreateShaderResourceView("Snow_Texture", device, context, L"resources/textures/Snow_bc.jpg");
	entityManager->CreateShaderResourceView("Cliff_Texture", device, context, L"resources/textures/CliffLayered_bc.tif");
	entityManager->CreateShaderResourceView("Cliff_Normal_Texture", device, context, L"resources/textures/CliffLayered_normal.tif");

	// Define the anisotropic filtering sampler description
	D3D11_SAMPLER_DESC samplerDesc = {}; // Zero out all values initially
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the U axis
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the V axis
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the W axis
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; // Use Anisotropic filtering
	samplerDesc.MaxAnisotropy = 16; // Use x16 anisotropy
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // This value needs to be higher than 0 for mipmapping to work

	// Create the anisotropic filtering sampler state
	entityManager->CreateSamplerState("Anisotropic_Sampler", device, samplerDesc);

	// Create the rock material using the previously set up resources
	entityManager->CreateMaterialWithNormal("Cliff_Normal_Material", "Normals_Vertex_Shader", "Normals_Pixel_Shader", "Cliff_Texture", "Cliff_Normal_Texture", "Anisotropic_Sampler");
	entityManager->CreateMaterial("Cliff_Material", "Default_Vertex_Shader", "Default_Pixel_Shader", "Cliff_Texture", "Anisotropic_Sampler");
	entityManager->CreateMaterial("Rock_Material", "Default_Vertex_Shader", "Default_Pixel_Shader", "Gravel_Texture", "Anisotropic_Sampler");
	entityManager->CreateMaterial("Snow_Material", "Default_Vertex_Shader", "Default_Pixel_Shader", "Snow_Texture", "Anisotropic_Sampler");

	// Load geometry
	entityManager->CreateMesh("Sphere_Mesh", device, "resources/models/sphere.obj");
	entityManager->CreateMesh("Helix_Mesh", device, "resources/models/helix.obj");
	entityManager->CreateMesh("Torus_Mesh", device, "resources/models/torus.obj");
	entityManager->CreateMesh("Cone_Mesh", device, "resources/models/cone.obj");

	// Create entities using the previously set up resources
	entityManager->CreateEntity("Player", "Sphere_Mesh", "Cliff_Normal_Material");
	/*entityManager->CreateEntity("Sphere_01", "Sphere_Mesh", "Cliff_Material");
	entityManager->CreateEntity("Sphere_02", "Sphere_Mesh", "Cliff_Normal_Material");*/
	entityManager->CreateEntity("Sphere_03", "Sphere_Mesh", "Cliff_Normal_Material");

	// Set up initial entity positions
	entityManager->GetEntity("Player")->SetScale(XMFLOAT3(0.5, 0.5, 0.5));
	entityManager->GetEntity("Player")->SetPosition(XMFLOAT3(-1.0, 0, 0));
	/*entityManager->GetEntity("Sphere_01")->SetPosition(XMFLOAT3(1, -2.5, 0));
	entityManager->GetEntity("Sphere_02")->SetPosition(XMFLOAT3(-1, -2.5, 0));*/
	entityManager->GetEntity("Sphere_03")->SetPosition(XMFLOAT3(-3, 0, 0));
	entityManager->GetEntity("Sphere_03")->SetScale(XMFLOAT3(0.25, 0.25, 0.25));
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	if (currentScene == SceneState::Game)
	{
		// Switch between normal and debug camera modes when the ` key is pressed
		static bool currentPress = false;
		if (GetAsyncKeyState(0xC0) & 0x8000)
		{
			if (!currentPress)
			{
				debugCameraEnabled = !debugCameraEnabled;
			}
			currentPress = true;
		}
		else
		{
			currentPress = false;
		}

		// Movement for the player entity
		Entity* player = entityManager->GetEntity("Player");
		if (&player != nullptr)
		{
			// Set movement rate
			float speed = 5.0;

			if (GetAsyncKeyState('W') & 0x8000)
			{
				player->MoveForward(XMFLOAT3(0, 0, speed * deltaTime));
			}

			if (GetAsyncKeyState('S') & 0x8000)
			{
				player->MoveForward(XMFLOAT3(0, 0, -speed * deltaTime));
			}

			if (GetAsyncKeyState('A') & 0x8000)
			{
				player->RotateBy(XMFLOAT3(0, -speed * deltaTime, 0));
			}

			if (GetAsyncKeyState('D') & 0x8000)
			{
				player->RotateBy(XMFLOAT3(0, speed * deltaTime, 0));
			}
		}

		// Update the game based on the current game state
		switch (state)
		{
		case GameState::Debug:
			DebugUpdate(deltaTime, totalTime);
			break;
		case GameState::Game:
			GameUpdate(deltaTime, totalTime);
			break;
		}

		// Update the camera
		camera->Update(deltaTime, totalTime, entityManager->GetEntity("Player"), debugCameraEnabled);

		// Update all entities
		entityManager->UpdateEntities(deltaTime, totalTime);
	}
}

void Game::GameUpdate(float deltaTime, float totalTime)
{

}

void Game::DebugUpdate(float deltaTime, float totalTime)
{
	// Rotate the torus entities
	/*Entity* torus1 = entityManager->GetEntity("Torus_01");
	Entity* torus2 = entityManager->GetEntity("Torus_02");
	XMFLOAT3 currentRot = torus1->GetRotation();
	XMFLOAT3 newRot = XMFLOAT3(currentRot.x, currentRot.y, currentRot.z + (1 * deltaTime));
	torus1->SetRotation(newRot);
	torus2->SetRotation(newRot);*/

	// Set up the rate of LERP to pulse up and down each second
	//float rate = 0.5f;
	//if ((long)totalTime % 2 == 0)
	//{
	//	rate = totalTime - (long)totalTime;
	//}
	//else
	//{
	//	rate = 1 - (totalTime - (long)totalTime);
	//}

	//// Lerp the scale of the helix entity
	//Entity* helix = entityManager->GetEntity("Player");
	//XMFLOAT3 scale = XMFLOAT3(1.0, 1.0, 1.0);
	//XMFLOAT3 scaleMin = XMFLOAT3(0.75, 0.75, 0.75);
	//XMFLOAT3 scaleMax = XMFLOAT3(1.25, 1.25, 1.25);
	//XMStoreFloat3(&scale, XMVectorLerp(XMLoadFloat3(&scaleMin), XMLoadFloat3(&scaleMax), rate));
	//helix->SetScale(scale);
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our camera's projection matrix since the window size changed
	camera->ResizeWindow(width, height);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Draw each entity with lighting
	if (currentScene == SceneState::Game)
	{
		entityManager->DrawEntities(context, camera, lights, _countof(lights));
	}
	else
	{
		menuManager->DisplayMainMenu(spriteBatch);
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}

#pragma region Mouse Input
// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	mouseDown = true;

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	mouseDown = false;

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Initialize the previous mouse positions at start so they aren't super large values
	static bool initialized = false;
	if (!initialized)
	{
		prevMousePos.x = x;
		prevMousePos.y = y;
		initialized = true;
	}

	// Set the sensitivity of the camera
	static float sensitivity = .1f;

	// Only rotate the camera if the mouse is clicked and the debug camera is enabled
	if (mouseDown && debugCameraEnabled)
	{
		// Rotate the camera
		float deltaX = (x - prevMousePos.x) * (3.141592f / 180.0f);
		float deltaY = (y - prevMousePos.y) * (3.141592f / 180.0f);
		camera->Rotate(deltaY * sensitivity, deltaX * sensitivity);
	}

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion