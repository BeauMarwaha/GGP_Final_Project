#pragma once

#include <map>
#include "Entity.h"
#include "Mesh.h"
#include "Material.h"

#pragma region Smart Structs
// Struct representing a smart mesh
struct SmartMesh
{
	SmartMesh(Mesh* mesh, unsigned int refCount) : mesh(mesh), refCount(refCount) { }
	Mesh* mesh; // Mesh Pointer
	unsigned int refCount; // Number of references to this mesh
};

// Struct representing a smart material
struct SmartMaterial
{
	SmartMaterial(Material* material, unsigned int refCount) : material(material), refCount(refCount) { }
	Material* material; // Material Pointer
	unsigned int refCount; // Number of references to this material
};

// Struct representing a smart simple vertex shader
struct SmartVertexShader
{
	SmartVertexShader(SimpleVertexShader* vertexShader, unsigned int refCount) : vertexShader(vertexShader), refCount(refCount) { }
	SimpleVertexShader* vertexShader; // Simple Vertex Shader Pointer
	unsigned int refCount; // Number of references to this material
};

// Struct representing a smart simple pixel shader
struct SmartPixelShader
{
	SmartPixelShader(SimplePixelShader* pixelShader, unsigned int refCount) : pixelShader(pixelShader), refCount(refCount) { }
	SimplePixelShader* pixelShader; // Simple Pixel Shader Pointer
	unsigned int refCount; // Number of references to this material
};

// Struct representing a smart shader resource view
struct SmartShaderResourceView
{
	SmartShaderResourceView(ID3D11ShaderResourceView* shaderResourceView, unsigned int refCount) : shaderResourceView(shaderResourceView), refCount(refCount) { }
	ID3D11ShaderResourceView* shaderResourceView; // DXTK Shader Reource View Pointer
	unsigned int refCount; // Number of references to this material
};

// Struct representing a smart sampler state
struct SmartSamplerState
{
	SmartSamplerState(ID3D11SamplerState* samplerState, unsigned int refCount) : samplerState(samplerState), refCount(refCount) { }
	ID3D11SamplerState* samplerState; // DXTK Shader Reource View Pointer
	unsigned int refCount; // Number of references to this material
};
#pragma endregion

class EntityManager
{
public:
	EntityManager(); // Constructor
	~EntityManager(); // Deconstructor

	#pragma region Helper Methods
	// Entity Helper Methods
	void CreateEntity(std::string entityName, Mesh* mesh, Material* material);
	void RemoveEntity(std::string entityName);
	Entity* GetEntity(std::string entityName);

	// Mesh Helper Methods
	void CreateMesh(std::string meshName, ID3D11Device* device, char* objFile);
	void RemoveMesh(std::string meshName);
	Mesh* GetMesh(std::string meshName);

	// Material Helper Methods
	void CreateMaterial(std::string materialName, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* shaderResourceView, ID3D11SamplerState* samplerState);
	void RemoveMaterial(std::string materialName);
	Material* GetMaterial(std::string materialName);

	// Vertex Shader Helper Methods
	void CreateVertexShader(std::string vertexShaderName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile);
	void RemoveVertexShader(std::string vertexShaderName);
	SmartVertexShader* GetVertexShader(std::string vertexShaderName);

	// Vertex Shader Helper Methods
	void CreatePixelShader(std::string pixelShaderName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile);
	void RemovePixelShader(std::string pixelShaderName);
	SmartPixelShader* GetPixelShader(std::string pixelShaderName);

	// Shader Resource View Helper Methods
	void CreateShaderResourceView(std::string shaderResourceViewName, Mesh* mesh, Material* material);
	void RemoveShaderResourceView(std::string shaderResourceViewName);
	ID3D11ShaderResourceView* GetShaderResourceView(std::string shaderResourceViewName);

	// Sampler State Helper Methods
	void CreateSamplerState(std::string samplerStateName, Mesh* mesh, Material* material);
	void RemoveSamplerState(std::string samplerStateName);
	ID3D11SamplerState* GetSamplerState(std::string samplerStateName);
	#pragma endregion

private:
	// Map of all entities handled in the manager (Uses entity name for the key)
	std::map<std::string, Entity*> entities;

	// Maps to keep track of entity related objects
	std::map<std::string, SmartMesh> meshes; // Meshes Map (Uses mesh name for the key)
	std::map<std::string, SmartMaterial> materials; // Materials Map (Uses material name for the key)
};