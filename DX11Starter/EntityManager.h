#pragma once

#include <map>
#include "Entity.h"
#include "Mesh.h"
#include "Material.h"
#include "WICTextureLoader.h"

#pragma region Smart Structs
// Struct representing a smart entity
struct SmartEntity
{
	SmartEntity(Entity* entity, std::string meshName, std::string materialName) : entity(entity), meshName(meshName), materialName(materialName) { }
	Entity* entity; // Entity Pointer
	std::string meshName; // Name of the mesh this entity utilizes
	std::string materialName; // Name of the material this entity utilizes
};

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
	std::string vertexShaderName; // Name of the vertex shader this material utilizes
	std::string pixelShaderName; // Name of the pixel shader this material utilizes
	std::string shaderResourceViewName; // Name of the shader resource view this material utilizes
	std::string samplerStateName; // Name of the sampler state this material utilizes
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

	#pragma region Public Helper Methods
	// Entity Helper Methods
	void CreateEntity(std::string entityName, std::string meshName, std::string materialName);
	void RemoveEntity(std::string entityName);

	// Mesh Helper Methods
	void CreateMesh(std::string meshName, ID3D11Device* device, char* objFile);
	void RemoveMesh(std::string meshName);

	// Material Helper Methods
	void CreateMaterial(std::string materialName, std::string vertexShaderName, std::string pixelShaderName, std::string shaderResourceViewName, std::string samplerStateName);
	void RemoveMaterial(std::string materialName);

	// Vertex Shader Helper Methods
	void CreateVertexShader(std::string vertexShaderName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile);
	void RemoveVertexShader(std::string vertexShaderName);

	// Vertex Shader Helper Methods
	void CreatePixelShader(std::string pixelShaderName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile);
	void RemovePixelShader(std::string pixelShaderName);

	// Shader Resource View Helper Methods
	void CreateShaderResourceView(std::string shaderResourceViewName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile);
	void RemoveShaderResourceView(std::string shaderResourceViewName);

	// Sampler State Helper Methods
	void CreateSamplerState(std::string samplerStateName, ID3D11Device* device, D3D11_SAMPLER_DESC samplerDesc);
	void RemoveSamplerState(std::string samplerStateName);
	#pragma endregion

private:
	// Map of all smart entities handled in the manager (Uses entity name for the key)
	std::map<std::string, SmartEntity> entities;

	// Maps to keep track of entity related objects
	std::map<std::string, SmartMesh> meshes; // Smart Meshes Map (Uses mesh name for the key)
	std::map<std::string, SmartMaterial> materials; // Smart Materials Map (Uses material name for the key)
	std::map<std::string, SmartVertexShader> vertexShaders; // Smart Vertex Shaders Map (Uses vertex shader name for the key)
	std::map<std::string, SmartPixelShader> pixelShaders; // Smart Pixel Shaders Map (Uses pixel shader name for the key)
	std::map<std::string, SmartShaderResourceView> shaderResourceViews; // Smart Shader Resource Views Map (Uses shader resource view name for the key)
	std::map<std::string, SmartSamplerState> samplerStates; // Smart Sampler States Map (Uses sampler state name for the key)

	#pragma region Private Helper Methods
	// Entity Helper Methods
	Entity* GetEntity(std::string entityName);

	// Mesh Helper Methods
	Mesh* GetMesh(std::string meshName);

	// Material Helper Methods
	Material* GetMaterial(std::string materialName);

	// Vertex Shader Helper Methods
	SimpleVertexShader* GetVertexShader(std::string vertexShaderName);

	// Vertex Shader Helper Methods
	SimplePixelShader* GetPixelShader(std::string pixelShaderName);

	// Shader Resource View Helper Methods
	ID3D11ShaderResourceView* GetShaderResourceView(std::string shaderResourceViewName);

	// Sampler State Helper Methods
	ID3D11SamplerState* GetSamplerState(std::string samplerStateName);
	#pragma endregion
};