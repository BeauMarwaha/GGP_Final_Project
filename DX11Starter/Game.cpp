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
	meshes = std::vector<Mesh*>();
	entities = std::vector<Entity>();
	camera = new Camera(width, height);
	vertexShader = nullptr;
	pixelShader = nullptr;
	material = nullptr;

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

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;

	// Release DXTK Texture resources
	shaderResourceView->Release();
	samplerState->Release();

	// Delete the basic material
	delete material;

	// Delete all entrys in the Mesh Pointer Vector Collection
	for (std::vector<Mesh>::size_type i = 0; i != meshes.size(); i++) {
		delete meshes[i];
	}
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Set up the directional light source
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

	// Helper methods for loading materials, creating some basic
	// geometry to draw, and some loading models
	//  - You'll be expanding and/or replacing these later
	LoadMaterials();
	CreateBasicGeometry();
	LoadModels();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Creates basic materials to be used in engine, this includes:
// - Loading shaders from compiled shader object (.cso) files using
//   my SimpleShader wrapper for DirectX shader manipulation
//   - SimpleShader provides helpful methods for sending
//     data to individual variables on the GPU
// - Using DirectXTK to load a texture from an external file
// - And creating a sampler description which specifies how the 
//   material should sample from the loaded texture
// --------------------------------------------------------
void Game::LoadMaterials()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");

	// Use the DirectXTK to load a texture from an external file and place it into a shader resource view
	CreateWICTextureFromFile(
		device,										// Application Device
		context,									// Application Device Context (necesary for auto generation of mipmaps)
		L"resources/textures/GravelCobble_bc.jpg",	// File path to external texture
		0,											// Reference to the texture which we don't need so we pass in 0
		&shaderResourceView);						// Address to the Shader Resource View pointer which we pass to the shader later

	// Define a sampler description
	D3D11_SAMPLER_DESC samplerDesc = D3D11_SAMPLER_DESC();
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the U axis
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the V axis
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the W axis
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Use trilinear filtering
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // This value needs to be higher than 0 for mipmapping to work

	// Create the sampler state using the defined sampler description
	device->CreateSamplerState(&samplerDesc, &samplerState);

	// Set up a material to be shared by all the basic mesh entities
	material = new Material(vertexShader, pixelShader, shaderResourceView, samplerState);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw 
// - For now this will be a few simple Mesh objects
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Set up the vertices and indices for Mesh 1 ---------------------------------
	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex vertices1[] =
	{
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT3(+0.0f, +1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f), XMFLOAT3(+1.5f, -1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), XMFLOAT3(-1.5f, -1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) }
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	unsigned int indices1[] = { 0, 1, 2 };

	// Calculate the number of vertices and indicies
	int vertexCount1 = sizeof(vertices1) / sizeof(vertices1[0]);
	int indexCount1 = sizeof(indices1) / sizeof(indices1[0]);

	// Create the actual Mesh object for Mesh 1
	meshes.push_back(new Mesh(device, vertices1, vertexCount1, indices1, indexCount1));
	
	// Set up the vertices and indices for Mesh 2 ---------------------------------
	Vertex vertices2[] =
	{
		{ XMFLOAT3(+2.0f, +1.0f, +0.0f), XMFLOAT3(+2.0f, +1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(+3.5f, -1.0f, +0.0f), XMFLOAT3(+3.5f, -1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(+2.0f, -1.0f, +0.0f), XMFLOAT3(+2.0f, -1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(+3.5f, +1.0f, +0.0f), XMFLOAT3(+3.5f, +1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) }
	};
	unsigned int indices2[] = { 0, 1, 2, 0, 3, 1 };

	// Calculate the number of vertices and indicies
	int vertexCount2 = sizeof(vertices2) / sizeof(vertices2[0]);
	int indexCount2 = sizeof(indices2) / sizeof(indices2[0]);

	// Create the actual Mesh object for Mesh 1
	meshes.push_back(new Mesh(device, vertices2, vertexCount2, indices2, indexCount2));

	// Set up the vertices and indices for Mesh 3 ---------------------------------
	Vertex vertices3[] =
	{
		{ XMFLOAT3(-2.0f, +1.0f, +0.0f), XMFLOAT3(-2.0f, +1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(-2.0f, -1.0f, +0.0f), XMFLOAT3(-2.0f, -1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(-3.5f, -1.0f, +0.0f), XMFLOAT3(-3.5f, -1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(-3.5f, +1.0f, +0.0f), XMFLOAT3(-3.5f, +1.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) }
	};
	unsigned int indices3[] = { 0, 1, 2, 3, 0, 2 };

	// Calculate the number of vertices and indicies
	int vertexCount3 = sizeof(vertices3) / sizeof(vertices3[0]);
	int indexCount3 = sizeof(indices3) / sizeof(indices3[0]);

	// Create the actual Mesh object for Mesh 1
	meshes.push_back(new Mesh(device, vertices3, vertexCount3, indices3, indexCount3));

	// Assign the created meshes and material to new entities
	entities.push_back(Entity(meshes[0], material));
	entities.push_back(Entity(meshes[0], material));
	entities.push_back(Entity(meshes[1], material));
	entities.push_back(Entity(meshes[2], material));
	entities.push_back(Entity(meshes[1], material));
	entities.push_back(Entity(meshes[2], material));
}

void Game::LoadModels()
{
	// Load meshes for models from external OBJ files
	meshes.push_back(new Mesh(device, "resources/models/helix.obj"));

	// Load meshes for models from external OBJ files
	meshes.push_back(new Mesh(device, "resources/models/torus.obj"));

	// Load meshes for models from external OBJ files
	meshes.push_back(new Mesh(device, "resources/models/cone.obj"));

	// Assign the created meshes and material to new entities
	entities.push_back(Entity(meshes[3], material));
	entities.push_back(Entity(meshes[3], material));
	entities.push_back(Entity(meshes[4], material));
	entities.push_back(Entity(meshes[5], material));

	// Move the new entities off to the side of the screen
	entities[6].MoveForward(XMFLOAT3(3, 0, 0));
	entities[7].MoveForward(XMFLOAT3(-3, 0, 0));
	entities[8].MoveForward(XMFLOAT3(-2, 2, 0));
	entities[9].MoveForward(XMFLOAT3(2, 2, 0));
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
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	// Movement for entity 0
	if (&entities[0] != nullptr)
	{
		// Set movement rate
		float speed = 5.0;

		if (GetAsyncKeyState('I') & 0x8000)
		{
			entities[0].MoveForward(XMFLOAT3(0, speed * deltaTime, 0));
		}

		if (GetAsyncKeyState('K') & 0x8000)
		{
			entities[0].MoveForward(XMFLOAT3(0, -speed * deltaTime, 0));
		}

		if (GetAsyncKeyState('L') & 0x8000)
		{
			entities[0].MoveForward(XMFLOAT3(speed * deltaTime, 0, 0));
		}

		if (GetAsyncKeyState('J') & 0x8000)
		{
			entities[0].MoveForward(XMFLOAT3(-speed * deltaTime, 0, 0));
		}

		if (GetAsyncKeyState('O') & 0x8000)
		{
			entities[0].MoveForward(XMFLOAT3(0, 0, speed * deltaTime));
		}

		if (GetAsyncKeyState('U') & 0x8000)
		{
			entities[0].MoveForward(XMFLOAT3(0, 0, -speed * deltaTime));
		}
	}

	// Rotate entity 1, 4, and 5
	XMFLOAT3 currentRot = entities[1].GetRotation();
	XMFLOAT3 newRot = XMFLOAT3(currentRot.x, currentRot.y, currentRot.z + (1 * deltaTime));
	entities[1].SetRotation(newRot);
	entities[4].SetRotation(newRot);
	entities[5].SetRotation(newRot);

	// Set up the rate of LERP to pulse up and down each second
	float rate = 0.5f;
	if ((long)totalTime % 2 == 0)
	{
		rate = totalTime - (long)totalTime;
	}
	else
	{
		rate = 1 - (totalTime - (long)totalTime);
	}

	// Lerp the scale of entity 2 and 3
	XMFLOAT3 scale = XMFLOAT3(1.0, 1.0, 1.0);
	XMFLOAT3 scaleMin = XMFLOAT3(0.75, 0.75, 0.75);
	XMFLOAT3 scaleMax = XMFLOAT3(1.25, 1.25, 1.25);
	XMStoreFloat3(&scale, XMVectorLerp(XMLoadFloat3(&scaleMin), XMLoadFloat3(&scaleMax), rate));
	entities[2].SetScale(scale);
	entities[3].SetScale(scale);

	// Update the camera
	camera->Update(deltaTime, totalTime);

	// Update all entities
	for (std::vector<Entity>::size_type i = 0; i != entities.size(); i++) 
	{
		entities[i].Update(deltaTime, totalTime);
	}
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

	// Pass the enviromental lights to the pixel shader for all objects
	pixelShader->SetData(
		"lights", // The name of the (eventual) variable in the shader
		&lights, // The address of the data to copy
		sizeof(DirectionalLight) * _countof(lights)); // The size of the data to copy

	// Draw each entity
	for (std::vector<Entity>::size_type i = 0; i != entities.size(); i++) {
		entities[i].Draw(context, camera->GetViewMatrix(), camera->GetProjectionMatrix());
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

	if (mouseDown)
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