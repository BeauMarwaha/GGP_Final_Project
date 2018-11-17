#include "EntityManager.h"
#include "Player.h"

// For the C++ standard library
using namespace std;

// For the DirectX Math library
using namespace DirectX;

EntityManager::EntityManager()
{
	// Instantiate the Maps
	entities = map<string, SmartEntity>();
	meshes = map<string, SmartMesh>();
	materials = map<string, SmartMaterial>();
	vertexShaders = map<string, SmartVertexShader>();
	pixelShaders = map<string, SmartPixelShader>();
	shaderResourceViews = map<string, SmartShaderResourceView>();
	samplerStates = map<string, SmartSamplerState>();
}

// Cleans up all remaing items in the manager
EntityManager::~EntityManager()
{
	// Create a vector to hold the names of resources to delete
	vector<string> names = vector<string>();

	// Get all existing entity names
	for (auto& entity : entities)
		names.push_back(entity.first);
	// Remove all existing entities
	for (std::vector<Entity>::size_type i = 0; i != names.size(); i++)
		RemoveEntity(names[i]);
	// Clear the list of names
	names.clear();

	// Get all existing mesh names
	for (auto& mesh : meshes)
		names.push_back(mesh.first);
	// Remove all existing meshes
	for (std::vector<Entity>::size_type i = 0; i != names.size(); i++)
		RemoveMesh(names[i]);
	// Clear the list of names
	names.clear();

	// Get all existing material names
	for (auto& material : materials)
		names.push_back(material.first);
	// Remove all existing materials
	for (std::vector<Entity>::size_type i = 0; i != names.size(); i++)
		RemoveMaterial(names[i]);
	// Clear the list of names
	names.clear();

	// Get all existing vertex shader names
	for (auto& vertexShader : vertexShaders)
		names.push_back(vertexShader.first);
	// Remove all existing vertex shaders
	for (std::vector<Entity>::size_type i = 0; i != names.size(); i++)
		RemoveVertexShader(names[i]);
	// Clear the list of names
	names.clear();

	// Get all existing pixel shader names
	for (auto& pixelShader : pixelShaders)
		names.push_back(pixelShader.first);
	// Remove all existing pixel shaders
	for (std::vector<Entity>::size_type i = 0; i != names.size(); i++)
		RemovePixelShader(names[i]);
	// Clear the list of names
	names.clear();

	// Get all existing shader resource view names
	for (auto& shaderResourceView : shaderResourceViews)
		names.push_back(shaderResourceView.first);
	// Remove all existing shader resource views
	for (std::vector<Entity>::size_type i = 0; i != names.size(); i++)
		RemoveShaderResourceView(names[i]);
	// Clear the list of names
	names.clear();

	// Get all existing sampler state names
	for (auto& samplerState : samplerStates)
		names.push_back(samplerState.first);
	// Remove all existing sampler states
	for (std::vector<Entity>::size_type i = 0; i != names.size(); i++)
		RemoveSamplerState(names[i]);
	// Clear the list of names
	names.clear();
}

bool EntityManager::UpdateEntities(float deltaTime, float totalTime)
{
	// Run the update method on all entities
	for (auto& entity : entities)
	{
		entity.second.entity->Update(deltaTime, totalTime);

		for (auto& other : entities)
		{
			// if entities are colliding with each other
			if (CheckForCollision(entity.second.entity, other.second.entity))
			{
				// TODO: Handle Collision
				if (entity.second.entity->GetType() == (int)EntityType::Asteroid)
				{
					if (other.second.entity->GetType() == (int)EntityType::Bullet)
					{
						// Bullet vs. Asteroid Collision -- Destroy both of them
						RemoveEntity(entity.first);
						RemoveEntity(other.first);

						/*if (meshes[entities[entity.first].meshName].refCount <= 0) return true;
						else*/ return false;
					}
				}
				if (entity.second.entity->GetType() == (int)EntityType::Player)
				{
					if (other.second.entity->GetType() == (int)EntityType::Asteroid)
					{
						// Player vs. Asteroid Collision -- signal to change scenes
						return true;
					}
				}
				//cout << entity.first << " is colliding with " << other.first << endl;
			}
		}
	}
	return false;
}

void EntityManager::DrawEntities(ID3D11DeviceContext* context, Camera* camera, DirectionalLight lights[], int lightCount, ID3D11ShaderResourceView* skySRV)
{
	// Draws all entities with lighting
	for (auto& entity : entities)
	{
		// Pass the enviromental lights to the pixel shader
		SimplePixelShader* pixelShader = pixelShaders[materials[entity.second.materialName].pixelShaderName].pixelShader;
		pixelShader->SetData(
			"lights", // The name of the variable in the shader
			lights, // The address of the data to copy
			sizeof(DirectionalLight) * lightCount); // The size of the data to copy

		// If this is the interior mapping material pass in unique pixel shader data
		if (entity.second.materialName == "InteriorMapping_Material")
		{
			pixelShader->SetShaderResourceView("SkyCube", skySRV);
			pixelShader->SetFloat3("CameraPosition", camera->GetPosition());
			pixelShader->SetFloat("Offices", 10);
			pixelShader->SetInt("NumCubeMaps", 8);
			pixelShader->SetInt("RandSeed", 11);
		}

		// Draw the entity
		entity.second.entity->Draw(context, camera->GetViewMatrix(), camera->GetProjectionMatrix());
	}
}

void EntityManager::CreateEntity(string entityName, string meshName, string materialName, EntityType type)
{
	// Ensure the specfied mesh exists
	if (meshes.count(meshName) == 0)
	{
		throw "The specified mesh: " + meshName + " does not exist.";
	}

	// Ensure the specfied material exists
	if (materials.count(materialName) == 0)
	{
		throw "The specified material: " + materialName + " does not exist.";
	}

	switch (type) {
	case EntityType::Asteroid:
		{
			// Create a new asteroid using the given mesh and material and assign it to the entity map
			entities[entityName] = SmartEntity(
				new Asteroid(
					GetMesh(meshName),
					GetMaterial(materialName),
					(int)EntityType::Asteroid
				),
				meshName,
				materialName);
		}
		break;
	case EntityType::Base:
		{
			// Create a new entity using the given mesh and material and assign it to the entity map
			entities[entityName] = SmartEntity(
				new Entity(
					GetMesh(meshName),
					GetMaterial(materialName),
					(int)EntityType::Base
				),
				meshName,
				materialName);
		}
		break;
	case EntityType::Bullet:
		{
			// Create a new bullet using the given mesh and material and assign it to the entity map
			entities[entityName] = SmartEntity(
				new Bullet(
					GetMesh(meshName),
					GetMaterial(materialName),
					(int)EntityType::Bullet
				),
				meshName,
				materialName);

			// Set the bullet's position to be slightly in front of the Player and direction to be the same as the Player's
			XMMATRIX rotation = XMMatrixRotationRollPitchYaw(GetEntity("Player")->GetRotation().x, GetEntity("Player")->GetRotation().y, 0);
			XMFLOAT4 forward = XMFLOAT4(0, 0, 1, 0);
			XMVECTOR newForward = XMVector4Transform(XMLoadFloat4(&forward), rotation);
			XMFLOAT4 playerForward = XMFLOAT4(0, 0, 1, 0);
			XMStoreFloat4(&playerForward, newForward);

			XMFLOAT3 initialPosition = GetEntity("Player")->GetPosition();
			initialPosition.x += playerForward.x * 3;
			initialPosition.z += playerForward.z * 3;
			entities[entityName].entity->SetPosition(initialPosition);
			entities[entityName].entity->SetDirection(GetEntity("Player")->GetDirection());
		}
		break;
	case EntityType::Player:
		{
			// Create a new player using the given mesh and material and assign it to the entity map
			entities[entityName] = SmartEntity(
				new Player(
					GetMesh(meshName),
					GetMaterial(materialName),
					(int)EntityType::Player
				),
				meshName,
				materialName);
			Player* play = (Player*)GetEntity("Player");
			play->SetEntityManager(this);
		}
		break;
	}
}

void EntityManager::RemoveEntity(string entityName)
{
	// Ensure the specfied entity exists
	if (entities.count(entityName) == 0)
	{
		throw "The specified entity: " + entityName + " does not exist.";
	}

	// Decrement the mesh and material reference counts for this entity
	meshes[entities[entityName].meshName].refCount--;
	materials[entities[entityName].materialName].refCount--;

	// Delete the entity instance from the heap
	delete entities[entityName].entity;

	// Remove the entity pair from the map
	entities.erase(entityName);
}

Entity* EntityManager::GetEntity(string entityName)
{
	// Ensure the specfied entity exists
	if (entities.count(entityName) == 0)
	{
		throw "The specified entity: " + entityName + " does not exist.";
	}

	// Return the specified entity instance
	return entities[entityName].entity;
}

void EntityManager::CreateMesh(string meshName, ID3D11Device* device, char* objFile)
{
	// Create a new smart mesh using the passed in parameters and assign it to the mesh map
	meshes[meshName] = SmartMesh(new Mesh(device, objFile), 0);
}

void EntityManager::RemoveMesh(string meshName)
{
	// Ensure the specfied mesh exists
	if (meshes.count(meshName) == 0)
	{
		throw "The specified mesh: " + meshName + " does not exist.";
	}

	// Ensure that the mesh is not currently being referenced
	if (meshes[meshName].refCount != 0)
	{
		throw "The specified mesh: " + meshName + " is still being referenced in memory and therefore can not be removed.";
	}

	// Delete the mesh instance from the heap
	delete meshes[meshName].mesh;

	// Remove the mesh pair from the map
	meshes.erase(meshName);
}

Mesh* EntityManager::GetMesh(string meshName)
{
	// Ensure the specfied mesh exists
	if (meshes.count(meshName) == 0)
	{
		throw "The specified mesh: " + meshName + " does not exist.";
	}

	// Increment the reference count for the smart mesh
	meshes[meshName].refCount++;

	// Return the requested mesh
	return meshes[meshName].mesh;
}

void EntityManager::CreateMaterial(string materialName, string vertexShaderName, string pixelShaderName, string shaderResourceViewBaseColorName, string samplerStateName)
{
	// Ensure the specfied vertex shader exists
	if (vertexShaders.count(vertexShaderName) == 0)
	{
		throw "The specified vertex shader: " + vertexShaderName + " does not exist.";
	}

	// Ensure the specfied pixel shader exists
	if (pixelShaders.count(pixelShaderName) == 0)
	{
		throw "The specified pixel shader: " + pixelShaderName + " does not exist.";
	}

	// Ensure the specfied shader resource view exists
	if (shaderResourceViews.count(shaderResourceViewBaseColorName) == 0)
	{
		throw "The specified shader resource view base color: " + shaderResourceViewBaseColorName + " does not exist.";
	}

	// Ensure the specfied sampler state exists
	if (samplerStates.count(samplerStateName) == 0)
	{
		throw "The specified sampler state: " + samplerStateName + " does not exist.";
	}

	// Create a new material using the passed in data and assign it to the material map
	materials[materialName] = SmartMaterial(
		new Material(
			GetVertexShader(vertexShaderName),
			GetPixelShader(pixelShaderName),
			GetShaderResourceView(shaderResourceViewBaseColorName),
			nullptr,
			GetSamplerState(samplerStateName)
		),
		0,
		vertexShaderName,
		pixelShaderName,
		shaderResourceViewBaseColorName,
		"NO_NORMAL",
		samplerStateName);
}

void EntityManager::CreateMaterialWithNormal(string materialName, string vertexShaderName, string pixelShaderName, std::string shaderResourceViewBaseColorName, std::string shaderResourceViewNormalName, string samplerStateName)
{
	// Ensure the specfied vertex shader exists
	if (vertexShaders.count(vertexShaderName) == 0)
	{
		throw "The specified vertex shader: " + vertexShaderName + " does not exist.";
	}

	// Ensure the specfied pixel shader exists
	if (pixelShaders.count(pixelShaderName) == 0)
	{
		throw "The specified pixel shader: " + pixelShaderName + " does not exist.";
	}

	// Ensure the specfied shader resource view base color exists
	if (shaderResourceViews.count(shaderResourceViewBaseColorName) == 0)
	{
		throw "The specified shader resource view base color: " + shaderResourceViewBaseColorName + " does not exist.";
	}

	// Ensure the specfied sampler state exists
	if (samplerStates.count(samplerStateName) == 0)
	{
		throw "The specified sampler state: " + samplerStateName + " does not exist.";
	}

	// Create a new material using the passed in data and assign it to the material map
	materials[materialName] = SmartMaterial(
		new Material(
			GetVertexShader(vertexShaderName),
			GetPixelShader(pixelShaderName),
			GetShaderResourceView(shaderResourceViewBaseColorName),
			GetShaderResourceView(shaderResourceViewNormalName),
			GetSamplerState(samplerStateName)
		),
		0,
		vertexShaderName,
		pixelShaderName,
		shaderResourceViewBaseColorName,
		shaderResourceViewNormalName,
		samplerStateName);
}

void EntityManager::RemoveMaterial(string materialName)
{
	// Ensure the specfied material exists
	if (materials.count(materialName) == 0)
	{
		throw "The specified material: " + materialName + " does not exist.";
	}

	// Ensure that the material is not currently being referenced
	if (materials[materialName].refCount != 0)
	{
		throw "The specified material: " + materialName + " is still being referenced in memory and therefore can not be removed.";
	}

	// Decrement the vertex shader, pixel shader, shader resource view, and sampler state reference counts for this material
	vertexShaders[materials[materialName].vertexShaderName].refCount--;
	pixelShaders[materials[materialName].pixelShaderName].refCount--;
	shaderResourceViews[materials[materialName].shaderResourceViewBaseColorName].refCount--;
	shaderResourceViews[materials[materialName].shaderResourceViewNormalName].refCount--;
	samplerStates[materials[materialName].samplerStateName].refCount--;

	// Delete the material instance from the heap
	delete materials[materialName].material;

	// Remove the material pair from the map
	materials.erase(materialName);
}

Material* EntityManager::GetMaterial(string materialName)
{
	// Ensure the specfied material exists
	if (materials.count(materialName) == 0)
	{
		throw "The specified material: " + materialName + " does not exist.";
	}

	// Increment the reference count for the smart material
	materials[materialName].refCount++;

	// Return the requested material
	return materials[materialName].material;
}

void EntityManager::CreateVertexShader(string vertexShaderName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile)
{
	// Create a new vertex shader using the passed in data and assign it to the vertex shader map
	vertexShaders[vertexShaderName] = SmartVertexShader(new SimpleVertexShader(device, context), 0);
	vertexShaders[vertexShaderName].vertexShader->LoadShaderFile(shaderFile);
}

void EntityManager::RemoveVertexShader(string vertexShaderName)
{
	// Ensure the specfied vertex shader exists
	if (vertexShaders.count(vertexShaderName) == 0)
	{
		throw "The specified vertex shader: " + vertexShaderName + " does not exist.";
	}

	// Ensure that the vertex shader is not currently being referenced
	if (vertexShaders[vertexShaderName].refCount != 0)
	{
		throw "The specified vertex shader: " + vertexShaderName + " is still being referenced in memory and therefore can not be removed.";
	}

	// Delete the vertex shader instance from the heap
	delete vertexShaders[vertexShaderName].vertexShader;

	// Remove the vertex shader pair from the map
	vertexShaders.erase(vertexShaderName);
}

SimpleVertexShader* EntityManager::GetVertexShader(string vertexShaderName)
{
	// Ensure the specfied vertex shader exists
	if (vertexShaders.count(vertexShaderName) == 0)
	{
		throw "The specified vertex shader: " + vertexShaderName + " does not exist.";
	}

	// Increment the reference count for the smart vertex shader
	vertexShaders[vertexShaderName].refCount++;

	// Return the requested vertex shader
	return vertexShaders[vertexShaderName].vertexShader;
}

void EntityManager::CreatePixelShader(string pixelShaderName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile)
{
	// Create a new pixel shader using the passed in data and assign it to the pixel shader map
	pixelShaders[pixelShaderName] = SmartPixelShader(new SimplePixelShader(device, context), 0);
	pixelShaders[pixelShaderName].pixelShader->LoadShaderFile(shaderFile);
}

void EntityManager::RemovePixelShader(string pixelShaderName)
{
	// Ensure the specfied pixel shader exists
	if (pixelShaders.count(pixelShaderName) == 0)
	{
		throw "The specified pixel shader: " + pixelShaderName + " does not exist.";
	}

	// Ensure that the pixel shader is not currently being referenced
	if (pixelShaders[pixelShaderName].refCount != 0)
	{
		throw "The specified pixel shader: " + pixelShaderName + " is still being referenced in memory and therefore can not be removed.";
	}

	// Delete the pixel shader instance from the heap
	delete pixelShaders[pixelShaderName].pixelShader;

	// Remove the pixel shader pair from the map
	pixelShaders.erase(pixelShaderName);
}

SimplePixelShader* EntityManager::GetPixelShader(string pixelShaderName)
{
	// Ensure the specfied pixel shader exists
	if (pixelShaders.count(pixelShaderName) == 0)
	{
		throw "The specified pixel shader: " + pixelShaderName + " does not exist.";
	}

	// Increment the reference count for the smart pixel shader
	pixelShaders[pixelShaderName].refCount++;

	// Return the requested pixel shader
	return pixelShaders[pixelShaderName].pixelShader;
}

void EntityManager::CreateShaderResourceView(string shaderResourceViewName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile)
{
	// Use the DirectXTK to load a texture from an external file and place it into a shader resource view
	ID3D11ShaderResourceView* shaderResourceView;
	CreateWICTextureFromFile(
		device,										// Application Device
		context,									// Application Device Context (necesary for auto generation of mipmaps)
		textureFile,								// File path to external texture
		0,											// Reference to the texture which we don't need so we pass in 0
		&shaderResourceView);						// Address to the Shader Resource View pointer which we pass to the shader later

	// Create a new shader resource view using the passed in data and assign it to the shader resource view map
	shaderResourceViews[shaderResourceViewName] = SmartShaderResourceView(shaderResourceView, 0);
}

void EntityManager::CreateInteriorMappingDDSShaderResourceView(std::string shaderResourceViewName, ID3D11Device * device, ID3D11DeviceContext * context, LPCWSTR textureFile)
{
	// Load all of the interior cube maps
	int numCubeMaps = 8;
	ID3D11Resource** interiors = new ID3D11Resource*[numCubeMaps];
	ID3D11ShaderResourceView** infos = new ID3D11ShaderResourceView*[numCubeMaps];
	
	CreateDDSTextureFromFile(device, context, textureFile, &interiors[0], &infos[0]);
	CreateDDSTextureFromFile(device, context, L"resources/textures/InteriorMaps/OfficeCubeMapDark.dds", &interiors[1], &infos[1]);
	CreateDDSTextureFromFile(device, context, L"resources/textures/InteriorMaps/OfficeCubeMapBrick.dds", &interiors[2], &infos[2]);
	CreateDDSTextureFromFile(device, context, L"resources/textures/InteriorMaps/OfficeCubeMapBrickDark.dds", &interiors[3], &infos[3]);
	CreateDDSTextureFromFile(device, context, L"resources/textures/InteriorMaps/OfficeCubeMapBrown.dds", &interiors[4], &infos[4]);
	CreateDDSTextureFromFile(device, context, L"resources/textures/InteriorMaps/OfficeCubeMapBrownDark.dds", &interiors[5], &infos[5]);
	CreateDDSTextureFromFile(device, context, L"resources/textures/InteriorMaps/OfficeCubeMapWhiteboard.dds", &interiors[6], &infos[6]);
	CreateDDSTextureFromFile(device, context, L"resources/textures/InteriorMaps/OfficeCubeMapWhiteboardDark.dds", &interiors[7], &infos[7]);

	// Grab the desc of the info SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	infos[0]->GetDesc(&srvDesc);
	int mipLevels = srvDesc.Texture2DArray.MipLevels;

	// Create a cube map array
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6 * numCubeMaps; // 6 faces per cube * total cubes
	cubeDesc.Height = 256;
	cubeDesc.Width = 256;
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	cubeDesc.CPUAccessFlags = 0;
	cubeDesc.Format = srvDesc.Format; // Use the same format
	cubeDesc.MipLevels = mipLevels;
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;
	cubeDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* cubeArrayTexture;
	device->CreateTexture2D(&cubeDesc, 0, &cubeArrayTexture);

	D3D11_BOX box = {};
	box.top = 0;
	box.left = 0;
	box.bottom = 256;
	box.right = 256;
	box.front = 0;
	box.back = 1;

	// Copy all textures into this one
	for (int cube = 0; cube < numCubeMaps; cube++)
	{
		for (int face = 0; face < 6; face++)
		{
			for (int mip = 0; mip < mipLevels; mip++)
			{
				// Update the box for this mip
				box.right = max(1, (unsigned int)pow(2, mipLevels - mip - 1));
				box.bottom = box.right;

				// Copy
				context->CopySubresourceRegion(
					cubeArrayTexture,
					D3D11CalcSubresource(mip, cube * 6 + face, mipLevels),
					0, 0, 0,
					interiors[cube],
					D3D11CalcSubresource(mip, face, mipLevels),
					&box);
			}
		}
	}

	// Create the shader resource view that will be used to
	// access the cube map array in the shader
	D3D11_SHADER_RESOURCE_VIEW_DESC finalSRVDesc = {};
	finalSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	finalSRVDesc.TextureCubeArray.First2DArrayFace = 0;
	finalSRVDesc.TextureCubeArray.MipLevels = mipLevels;
	finalSRVDesc.TextureCubeArray.MostDetailedMip = 0;
	finalSRVDesc.TextureCubeArray.NumCubes = numCubeMaps;
	finalSRVDesc.Format = cubeDesc.Format;

	ID3D11ShaderResourceView* interiorCubeSRV;
	device->CreateShaderResourceView(cubeArrayTexture, &finalSRVDesc, &interiorCubeSRV);
	
	// Create a new shader resource view using the passed in data and assign it to the shader resource view map
	shaderResourceViews[shaderResourceViewName] = SmartShaderResourceView(interiorCubeSRV, 0);

	// Done!  Clean up everything we don't need
	cubeArrayTexture->Release();
	for (int i = 0; i < numCubeMaps; i++) {
		infos[i]->Release();
		interiors[i]->Release();
	}
	delete[] infos;
	delete[] interiors;
}

void EntityManager::RemoveShaderResourceView(string shaderResourceViewName)
{
	// Check to see if the shader resource view we are trying to remove is actually a non-existant normal map
	if (shaderResourceViewName == "NO_NORMAL")
	{
		// If so there is no memory allocated for this normal map so we need not do anything else
		return;
	}

	// Ensure the specfied shader resource view exists
	if (shaderResourceViews.count(shaderResourceViewName) == 0)
	{
		throw "The specified shader resource view: " + shaderResourceViewName + " does not exist.";
	}

	// Ensure that the shader resource view is not currently being referenced
	if (shaderResourceViews[shaderResourceViewName].refCount != 0)
	{
		throw "The specified shader resource view: " + shaderResourceViewName + " is still being referenced in memory and therefore can not be removed.";
	}

	// Release the shader resource view instance
	shaderResourceViews[shaderResourceViewName].shaderResourceView->Release();

	// Remove the shader resource view pair from the map
	shaderResourceViews.erase(shaderResourceViewName);
}

ID3D11ShaderResourceView* EntityManager::GetShaderResourceView(string shaderResourceViewName)
{
	// Ensure the specfied shader resource view exists
	if (shaderResourceViews.count(shaderResourceViewName) == 0)
	{
		throw "The specified shader resource view: " + shaderResourceViewName + " does not exist.";
	}

	// Increment the reference count for the smart shader resource view
	shaderResourceViews[shaderResourceViewName].refCount++;

	// Return the requested shader resource view
	return shaderResourceViews[shaderResourceViewName].shaderResourceView;
}

void EntityManager::CreateSamplerState(string samplerStateName, ID3D11Device* device, D3D11_SAMPLER_DESC samplerDesc)
{
	// Create the sampler state using the defined sampler description
	ID3D11SamplerState* samplerState;
	device->CreateSamplerState(&samplerDesc, &samplerState);

	// Create a new sampler state using the passed in data and assign it to the sampler state map
	samplerStates[samplerStateName] = SmartSamplerState(samplerState, 0);
}

void EntityManager::RemoveSamplerState(string samplerStateName)
{
	// Ensure the specfied sampler state exists
	if (samplerStates.count(samplerStateName) == 0)
	{
		throw "The specified sampler state: " + samplerStateName + " does not exist.";
	}

	// Ensure that the sampler state is not currently being referenced
	if (samplerStates[samplerStateName].refCount != 0)
	{
		throw "The specified sampler state: " + samplerStateName + " is still being referenced in memory and therefore can not be removed.";
	}

	// Release the sampler state instance
	samplerStates[samplerStateName].samplerState->Release();

	// Remove the sampler state pair from the map
	samplerStates.erase(samplerStateName);
}

ID3D11SamplerState* EntityManager::GetSamplerState(string samplerStateName)
{
	// Ensure the specfied sampler state exists
	if (samplerStates.count(samplerStateName) == 0)
	{
		throw "The specified sampler state: " + samplerStateName + " does not exist.";
	}

	// Increment the reference count for the smart sampler state
	samplerStates[samplerStateName].refCount++;

	// Return the requested sampler state
	return samplerStates[samplerStateName].samplerState;
}

bool EntityManager::CheckForCollision(Entity * entity1, Entity * entity2)
{
	if (!(entity1 == entity2) && entity1->GetCollider().GetEnabled() && entity2->GetCollider().GetEnabled())
	{
		float dx = entity1->GetPosition().x - entity2->GetPosition().x;
		float dz = entity1->GetPosition().z - entity2->GetPosition().z;
		float distance = sqrt(dx * dx + dz * dz);
		if (distance < entity1->GetCollider().GetRadius() + entity2->GetCollider().GetRadius())
		{
			return true;
		}
	}
	return false;
}
