#include "Game.h"
#include "Vertex.h"
#include <ctime> 

#include "DDSTextureLoader.h"

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
	state = GameState::Game;
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

	// Clean up the sky stuff
	sampler->Release();
	skySRV->Release();
	skyDepthState->Release();
	skyRastState->Release();
	delete skyVS;
	delete skyPS;
	delete skyMesh;

	// Clean up the post process stuff
	normalSRV->Release();
	brightSRV->Release();
	normalRTV->Release();
	brightRTV->Release();
	delete ppVS;
	delete extractPS;
	delete bloomPS;

	// Cease E_M_I_T
	particleBlendState->Release();
	particleDepthState->Release();

	// Delete the menu manager
	delete menuManager;

	// delete the font
	delete font;

	// Delete the sprite batch
	delete spriteBatch;

	// Delete the asteroid count
	delete asteroidCount;
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
	// Seed the random 
	srand((unsigned)time(0));

	/*context->RSGetState(&rasState);
	context->OMGetBlendState(&blendState, blendFactor, blendMask);*/

	// Helper methods for initialization based on game state
	switch (state)
	{
	case GameState::Debug:
		CreateDebugLights();
		CreateDebugEntities();
		CreateSky();
		break;
	case GameState::Game:
		CreateLights();
		CreateEntities();
		CreateSky();
		break;
	}


#pragma region Post Processing Setup
	// Create the vertex shader for post processing
	ppVS = new SimpleVertexShader(device, context);
	ppVS->LoadShaderFile(L"VertexShaderPostProcessing.cso");

	// Create the pixel shader that extracts the bright pixels
	extractPS = new SimplePixelShader(device, context);
	extractPS->LoadShaderFile(L"PixelShaderExtractBright.cso");

	// Create the pixel shader that blurs the bright pixels and adds them to the normal texture before post processing
	bloomPS = new SimplePixelShader(device, context);
	bloomPS->LoadShaderFile(L"PixelShaderBloom.cso");

	// Create post process resources -----------------------------------------
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* extractTexture;
	device->CreateTexture2D(&textureDesc, 0, &extractTexture);

	ID3D11Texture2D* bloomTexture;
	device->CreateTexture2D(&textureDesc, 0, &bloomTexture);

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(extractTexture, &rtvDesc, &normalRTV);
	device->CreateRenderTargetView(bloomTexture, &rtvDesc, &brightRTV);

	// Create the Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	device->CreateShaderResourceView(extractTexture, &srvDesc, &normalSRV);
	device->CreateShaderResourceView(bloomTexture, &srvDesc, &brightSRV);

	// We don't need the texture references themselves no mo'
	extractTexture->Release();
	bloomTexture->Release();
#pragma endregion Post Processing Setup

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
	lights[0].DiffuseColor = XMFLOAT4(.5, .5, .5, 1);
	lights[0].Direction = XMFLOAT3(1, -1, 0);
	lights[1].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[1].DiffuseColor = XMFLOAT4(.5, .5, .5, 1);
	lights[1].Direction = XMFLOAT3(-1, 1, 0);
	lights[2].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[2].DiffuseColor = XMFLOAT4(.5, .5, .5, 1);
	lights[2].Direction = XMFLOAT3(-1, -1, 0);
	lights[3].AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	lights[3].DiffuseColor = XMFLOAT4(.5, .5, .5, 1);
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
	entityManager->CreateVertexShader("Default_Vertex_Shader", device, context, L"VertexShader.cso");
	entityManager->CreateVertexShader("Normals_Vertex_Shader", device, context, L"VertexShaderNormals.cso");
	entityManager->CreateVertexShader("InteriorMapping_Vertex_Shader", device, context, L"VertexShaderInteriorMapping.cso");
	entityManager->CreateVertexShader("Particle_Vertex_Shader", device, context, L"VertexShaderParticle.cso");

	// Create the pixel shaders
	entityManager->CreatePixelShader("Default_Pixel_Shader", device, context, L"PixelShader.cso");
	entityManager->CreatePixelShader("Normals_Pixel_Shader", device, context, L"PixelShaderNormals.cso");
	entityManager->CreatePixelShader("InteriorMapping_Pixel_Shader", device, context, L"PixelShaderInteriorMapping.cso");
	entityManager->CreatePixelShader("Particle_Pixel_Shader", device, context, L"PixelShaderParticle.cso");

	// Create the shader resource views
	entityManager->CreateShaderResourceView("Cliff_Texture", device, context, L"resources/textures/CliffLayered_bc.tif");
	entityManager->CreateShaderResourceView("Cliff_Normal_Texture", device, context, L"resources/textures/CliffLayered_normal.tif");
	entityManager->CreateShaderResourceView("SpaceShip_Texture", device, context, L"resources/textures/SpaceShip/SpaceShip_bc.png");
	entityManager->CreateShaderResourceView("SpaceShip_Normal_Texture", device, context, L"resources/textures/SpaceShip/SpaceShip_normal.png");
	entityManager->CreateShaderResourceView("Bullet_Texture", device, context, L"resources/textures/Bullet_bc.png");

	// Create the interior mapping shader resource view
	LPCWSTR textureFiles[8];
	textureFiles[0] = L"resources/textures/InteriorMaps/OfficeCubeMap.dds";
	textureFiles[1] = L"resources/textures/InteriorMaps/OfficeCubeMapDark.dds";
	textureFiles[2] = L"resources/textures/InteriorMaps/OfficeCubeMapBrick.dds";
	textureFiles[3] = L"resources/textures/InteriorMaps/OfficeCubeMapBrickDark.dds";
	textureFiles[4] = L"resources/textures/InteriorMaps/OfficeCubeMapBrown.dds";
	textureFiles[5] = L"resources/textures/InteriorMaps/OfficeCubeMapBrownDark.dds";
	textureFiles[6] = L"resources/textures/InteriorMaps/OfficeCubeMapWhiteboard.dds";
	textureFiles[7] = L"resources/textures/InteriorMaps/OfficeCubeMapWhiteboardDark.dds";
	entityManager->CreateInteriorMappingDDSShaderResourceView("InteriorMap_Texture", device, context, textureFiles, 8);

	// Create particle textures
	//entityManager->CreateShaderResourceView("fireParticle", device, context, L"resources/textures/SpaceShip/fireParticle.jpg");
	entityManager->CreateShaderResourceView("Particle", device, context, L"resources/textures/particles/particle.jpg");

	// Define the anisotropic filtering sampler description
	D3D11_SAMPLER_DESC samplerDesc = {}; // Zero out all values initially
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the U axis
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the V axis
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the W axis
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; // Use Anisotropic filtering
	samplerDesc.MaxAnisotropy = 16; // Use x16 anisotropy
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // This value needs to be higher than 0 for mipmapping to work

	// A depth state for the particles
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &particleDepthState);

	// Blend for particles (additive)
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &particleBlendState);

	// Create the anisotropic filtering sampler state
	entityManager->CreateSamplerState("Anisotropic_Sampler", device, samplerDesc);

	// Create materials using the previously set up resources
	entityManager->CreateMaterialWithNormal("Asteroid_Material", "Normals_Vertex_Shader", "Normals_Pixel_Shader", "Cliff_Texture", "Cliff_Normal_Texture", "Anisotropic_Sampler");
	entityManager->CreateMaterialWithNormal("SpaceShip_Material", "Normals_Vertex_Shader", "Normals_Pixel_Shader", "SpaceShip_Texture", "SpaceShip_Normal_Texture", "Anisotropic_Sampler");
	entityManager->CreateMaterial("Bullet_Material", "Default_Vertex_Shader", "Default_Pixel_Shader", "Bullet_Texture", "Anisotropic_Sampler");
	entityManager->CreateMaterial("InteriorMapping_Material", "InteriorMapping_Vertex_Shader", "InteriorMapping_Pixel_Shader", "InteriorMap_Texture", "Anisotropic_Sampler");

	// Load geometry
	entityManager->CreateMesh("Sphere_Mesh", device, "resources/models/sphere.obj");
	entityManager->CreateMesh("SpaceShip_Mesh", device, "resources/models/SpaceShip.obj");
	entityManager->CreateMesh("Bullet_Mesh", device, "resources/models/bullet.obj");
	entityManager->CreateMesh("Building_Mesh_01", device, "resources/models/cube.obj");
	entityManager->CreateMesh("Building_Mesh_02", device, "resources/models/cube.obj");
	entityManager->CreateMesh("Building_Mesh_03", device, "resources/models/helix.obj");
	entityManager->CreateMesh("Building_Mesh_04", device, "resources/models/cylinder.obj");
	entityManager->CreateMesh("Building_Mesh_05", device, "resources/models/cylinder.obj");

	// Create emitters and pass them to entities
	entityManager->CreateEmitter("Exhaust_Emitter", device, "Particle_Vertex_Shader", "Particle_Pixel_Shader", "Particle", particleDepthState, particleBlendState);
	entityManager->CreateEmitter("Explosion_Emitter", device, "Particle_Vertex_Shader", "Particle_Pixel_Shader", "Particle", particleDepthState, particleBlendState);

	// declare properties for explosion emitter
	entityManager->GetEmitter("Explosion_Emitter")->SetParticlesPerSecod(0);
	entityManager->GetEmitter("Explosion_Emitter")->SetMaxParticles(300);
	entityManager->GetEmitter("Explosion_Emitter")->SetLifetime(1);
	entityManager->GetEmitter("Explosion_Emitter")->SetStartSize(0.1f);
	entityManager->GetEmitter("Explosion_Emitter")->SetEndSize(5.0f);
	entityManager->GetEmitter("Explosion_Emitter")->SetStartColor(XMFLOAT4(1.0f, 0.1f, 0.1f, 0.2f));
	entityManager->GetEmitter("Explosion_Emitter")->SetEndColor(XMFLOAT4(1.0f, 0.6f, 0.1f, 0.0f));
	entityManager->GetEmitter("Explosion_Emitter")->SetEmitterVelocity(XMFLOAT3(0, 5, 0));
	entityManager->GetEmitter("Explosion_Emitter")->SetEmitterPosition(XMFLOAT3(0, 0, 0));
	entityManager->GetEmitter("Explosion_Emitter")->SetEmitterAcceleration(XMFLOAT3(0, 0, 0));

	// Create entities using the previously set up resources
	entityManager->CreateEntityWithEmitter("Player", "SpaceShip_Mesh", "SpaceShip_Material", "Exhaust_Emitter", EntityType::Player);
	entityManager->CreateEntity("Asteroid1", "Sphere_Mesh", "Asteroid_Material", EntityType::Asteroid);
	entityManager->CreateEntity("Asteroid2", "Sphere_Mesh", "Asteroid_Material", EntityType::Asteroid);
	entityManager->CreateEntity("Asteroid3", "Sphere_Mesh", "Asteroid_Material", EntityType::Asteroid);
	entityManager->CreateEntity("Asteroid4", "Sphere_Mesh", "Asteroid_Material", EntityType::Asteroid);
	entityManager->CreateEntity("Asteroid5", "Sphere_Mesh", "Asteroid_Material", EntityType::Asteroid);

	// Manually set up asteroid count
	asteroidCount = new int();
	*asteroidCount = 5;

	// Create buildings utilizing interior mapping and randomly place them on the outskitrs of the scene
	std::vector<int> collidingBuildings = std::vector<int>();
	for (int i = 0; i < 100; i++)
	{
		// Create the building entity
		std::string name = "Building_" + std::to_string(i);
		entityManager->CreateEntity(name, "Building_Mesh_0" + std::to_string(rand() % 5 + 1), "InteriorMapping_Material", EntityType::Base);

		// Generate and set building entity scale and rotation
		float scale = rand() % 30 + 10;
		entityManager->GetEntity(name)->SetUniformScale(scale);
		entityManager->GetEntity(name)->SetRotation(XMFLOAT3(rand() % 180, rand() % 180, rand() % 180));

		// Generate and set the building's x and z position
		float x = 0;
		float z = 0;
		float minDist = 100;
		float scaleDistance = 150;

		// Generate a semi-random x and y co-ordinate for the building
		do
		{
			x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
			x *= scaleDistance;
			z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
			z *= scaleDistance;
		} while ((x < minDist && x > -minDist) && (z < minDist && z > -minDist));
		entityManager->GetEntity(name)->SetPosition(XMFLOAT3(x, 0, z));

		// Check to see if this building is colliding with any others using simple circle collision
		bool colliding = false;
		for (int j = i - 1; j >= 0; j--)
		{
			// Make sure the other building actually exists
			if (std::find(collidingBuildings.begin(), collidingBuildings.end(), j) == collidingBuildings.end()) {
				if (entityManager->CheckForCollision(entityManager->GetEntity(name), entityManager->GetEntity("Building_" + std::to_string(j))))
				{
					colliding = true;
				}
			}
		}

		// If the building entity is colliding, remove it
		if (colliding)
		{
			entityManager->RemoveEntity(name);
			collidingBuildings.push_back(i);
		}
	}
}

// --------------------------------------------------------
// Loads Vertex and Pixel shaders for the skybox
// Creates the Cubemap Texture
// Sets up a Rasterizer state and a Depth Stencil state for the skybox
// --------------------------------------------------------
void Game::CreateSky()
{
	skyVS = new SimpleVertexShader(device, context);
	skyVS->LoadShaderFile(L"VertexShaderSky.cso");

	skyPS = new SimplePixelShader(device, context);
	skyPS->LoadShaderFile(L"PixelShaderSky.cso");

	// Track a cube mesh separately from other meshes so its specific to the skybox and not entities
	skyMesh = new Mesh(device, "resources/models/cube.obj");

	// Define the anisotropic filtering sampler description
	D3D11_SAMPLER_DESC samplerDesc = {}; // Zero out all values initially
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the U axis
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the V axis
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Have UVW address wrap on the W axis
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; // Use Anisotropic filtering
	samplerDesc.MaxAnisotropy = 16; // Use x16 anisotropy
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // This value needs to be higher than 0 for mipmapping to work
	// Ask DirectX for the actual object
	device->CreateSamplerState(&samplerDesc, &sampler);

	// Texture
	CreateDDSTextureFromFile(device, context, L"resources/textures/skybox/SpaceCubeMap.dds", 0, &skySRV);

	// Rasterizer state for drawing the inside of my sky box geometry
	D3D11_RASTERIZER_DESC rs = {};
	rs.FillMode = D3D11_FILL_SOLID;
	rs.CullMode = D3D11_CULL_FRONT; // Reverse culling mode to be front-side so we see the interior of the skybox
	rs.DepthClipEnable = true;
	device->CreateRasterizerState(&rs, &skyRastState);

	D3D11_DEPTH_STENCIL_DESC ds = {};
	ds.DepthEnable = true;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Make the depth to be the max value always
	device->CreateDepthStencilState(&ds, &skyDepthState);
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
	entityManager->CreateEntity("Player", "Sphere_Mesh", "Cliff_Normal_Material", EntityType::Player);
	/*entityManager->CreateEntity("Sphere_01", "Sphere_Mesh", "Cliff_Material");
	entityManager->CreateEntity("Sphere_02", "Sphere_Mesh", "Cliff_Normal_Material");*/
	entityManager->CreateEntity("Sphere_03", "Sphere_Mesh", "Cliff_Normal_Material", EntityType::Base);

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
			static float moveSpeed = 5.0;
			static float turnSpeed = 1.0;

			if (GetAsyncKeyState('W') & 0x8000)
			{
				player->MoveForward(XMFLOAT3(0, 0, moveSpeed * deltaTime), 0);
			}

			if (GetAsyncKeyState('S') & 0x8000)
			{
				player->MoveForward(XMFLOAT3(0, 0, -moveSpeed * deltaTime), 0);
			}

			if (GetAsyncKeyState('A') & 0x8000)
			{
				player->RotateBy(XMFLOAT3(0, -turnSpeed * deltaTime, 0));
			}

			if (GetAsyncKeyState('D') & 0x8000)
			{
				player->RotateBy(XMFLOAT3(0, turnSpeed * deltaTime, 0));
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

		// Update the explosion emitter
		entityManager->GetEmitter("Explosion_Emitter")->Update(deltaTime);

		// Update all entities
		bool playerCollision = entityManager->UpdateEntities(deltaTime, totalTime, asteroidCount, entityManager->GetEmitter("Explosion_Emitter"));
		if (playerCollision)
		{
			currentScene = SceneState::GameOver;
		}
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
// Draws the skybox which includes
//  -Setting up the sky render states
//  -Getting the buffers for the mesh used
//  -Setting the buffers and passing in the camera matrices
//  -Sending in the texture info
//  -Actually drawing the sky
// --------------------------------------------------------
void Game::DrawSky()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Set up sky render states using the variables we initialized earlier
	context->RSSetState(skyRastState);
	context->OMSetDepthStencilState(skyDepthState, 0);

	// After drawing all of our regular (solid) objects, draw the sky!
	ID3D11Buffer* skyVB = skyMesh->GetVertexBuffer();
	ID3D11Buffer* skyIB = skyMesh->GetIndexBuffer();

	// Set the buffers
	context->IASetVertexBuffers(0, 1, &skyVB, &stride, &offset);
	context->IASetIndexBuffer(skyIB, DXGI_FORMAT_R32_UINT, 0);

	// Send in the view and projection matrices, don't need the world for the skybox
	skyVS->SetMatrix4x4("view", camera->GetViewMatrix());
	skyVS->SetMatrix4x4("projection", camera->GetProjectionMatrix());

	skyVS->CopyAllBufferData();
	skyVS->SetShader();

	// Send texture-related stuff
	skyPS->SetShaderResourceView("SkyTextureBase", skySRV);
	skyPS->SetSamplerState("basicSampler", sampler);

	skyPS->CopyAllBufferData(); // Remember to copy to the GPU!!!!
	skyPS->SetShader();

	// Finally do the actual drawing
	context->DrawIndexed(skyMesh->GetIndexCount(), 0, 0);

	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
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
	// Change to our "pre-post processing" render target (normal)
	context->OMSetRenderTargets(1, &normalRTV, depthStencilView);

	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(normalRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	
	switch (currentScene)
	{
		case SceneState::Game:
			// Display Game HUD
			

			// Draw each entity with lighting
			entityManager->DrawEntities(context, camera, lights, _countof(lights), skySRV);
			// Draw the sky after you finish drawing opaque objects
			DrawSky();
			menuManager->DisplayGameHUD(spriteBatch, context, *asteroidCount);

			// Draw player's emitter
			((Player *)entityManager->GetEntity("Player"))->DrawEmitter(context, camera->GetViewMatrix(), camera->GetProjectionMatrix());
			// draw the explosion emitter
			entityManager->GetEmitter("Explosion_Emitter")->Draw(context, camera->GetViewMatrix(), camera->GetProjectionMatrix());
			break;
		case SceneState::Main:
			// Draw the sky after you finish drawing opaque objects
			
			DrawSky();

			menuManager->DisplayMainMenu(spriteBatch, context);
			break;
		case SceneState::GameOver:
			// Draw the sky after you finish drawing opaque objects
			DrawSky();
			menuManager->DisplayGameOverMenu(spriteBatch, context);
			break;
	}
	// Reset any changed render states!
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);

	// Done with scene render - swap to the extracted bright pixels render target
	context->OMSetRenderTargets(1, &brightRTV, 0);

	// Post process draw ================================

	context->ClearRenderTargetView(brightRTV, color);

	// Set up my shaders
	ppVS->SetShader();
	extractPS->SetShader();
	extractPS->CopyAllBufferData();

	extractPS->SetShaderResourceView("Pixels", normalSRV);
	extractPS->SetSamplerState("Sampler", sampler);

	// Unbind vertex and index buffers!
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* nothing = 0;
	context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	// Draw exactly 3 vertices
	context->Draw(3, 0);

	// Done with scene render - swap back to the back buffer
	context->OMSetRenderTargets(1, &backBufferRTV, 0);
	context->ClearRenderTargetView(backBufferRTV, color);

	// Set up my shaders
	ppVS->SetShader();
	bloomPS->SetShader();
	bloomPS->SetInt("blurAmount", 5);
	bloomPS->SetFloat("pixelWidth", 1.0f / width);
	bloomPS->SetFloat("pixelHeight", 1.0f / height);
	bloomPS->CopyAllBufferData();

	bloomPS->SetShaderResourceView("Pixels", normalSRV);
	bloomPS->SetShaderResourceView("BrightPixels", brightSRV);
	bloomPS->SetSamplerState("Sampler", sampler);

	// Draw exactly 3 vertices
	context->Draw(3, 0);

	// Now that we're done, UNBIND the srv from the pixel shader
	extractPS->SetShaderResourceView("Pixels", 0);
	bloomPS->SetShaderResourceView("Pixels", 0);

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

	switch (currentScene)
	{
		case SceneState::Main:
			if (menuManager->DetectStartClick(x, y))
			{
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				// test making a new rasterizer state

				//blendDesc.RenderTarget

				currentScene = SceneState::Game;
			}
			if (menuManager->DetectQuitClick(x, y)) Quit();
			break;
		
		case SceneState::GameOver:
			if (menuManager->DetectQuitClick(x, y)) Quit();
			break;
	}

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