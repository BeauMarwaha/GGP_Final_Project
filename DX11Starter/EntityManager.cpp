#include "EntityManager.h"

// For the C++ standard library
using namespace std;

EntityManager::EntityManager()
{
	// Instantiate the Entity Map
	entities = map<string, Entity*>();
}

EntityManager::~EntityManager()
{
	// Delete all entity instances from the heap
	for (auto& entity : entities)
	{
		delete entity.second;
	}

	// Empty the entity map
	entities.clear();
}

void EntityManager::CreateEntity(string entityName, Mesh* mesh, Material* material)
{
	// create a new entity using the given mesh and material and assign it to the entity map
	entities[entityName] = new Entity(mesh, material);
}

void EntityManager::RemoveEntity(string entityName)
{
	// Delete the entity instance from the heap
	delete entities[entityName];

	// Remove the entity pair from the heap
	entities.erase(entityName);
}

Entity* EntityManager::GetEntity(string entityName)
{
	// Return the specified entity instance
	return entities[entityName];
}

void EntityManager::CreateMesh(string meshName, ID3D11Device* device, char* objFile)
{
}

void EntityManager::RemoveMesh(string meshName)
{
}

Mesh* EntityManager::GetMesh(string meshName)
{
	return nullptr;
}

void EntityManager::CreateMaterial(string materialName, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* shaderResourceView, ID3D11SamplerState* samplerState)
{
}

void EntityManager::RemoveMaterial(string materialName)
{
}

Material* EntityManager::GetMaterial(string materialName)
{
	return nullptr;
}

void EntityManager::CreateVertexShader(string vertexShaderName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile)
{
}

void EntityManager::RemoveVertexShader(string vertexShaderName)
{
}

SmartVertexShader* EntityManager::GetVertexShader(string vertexShaderName)
{
	return nullptr;
}

void EntityManager::CreatePixelShader(string pixelShaderName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile)
{
}

void EntityManager::RemovePixelShader(string pixelShaderName)
{
}

SmartPixelShader* EntityManager::GetPixelShader(string pixelShaderName)
{
	return nullptr;
}

void EntityManager::CreateShaderResourceView(string shaderResourceViewName, Mesh* mesh, Material* material)
{
}

void EntityManager::RemoveShaderResourceView(string shaderResourceViewName)
{
}

ID3D11ShaderResourceView* EntityManager::GetShaderResourceView(string shaderResourceViewName)
{
	return nullptr;
}

void EntityManager::CreateSamplerState(string samplerStateName, Mesh* mesh, Material* material)
{
}

void EntityManager::RemoveSamplerState(string samplerStateName)
{
}

ID3D11SamplerState* EntityManager::GetSamplerState(string samplerStateName)
{
	return nullptr;
}
