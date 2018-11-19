#pragma once
#ifndef EntityManager_Included
#define EntityManager_Included

#include <map>
#include <iostream>
#include "Entity.h"
#include "Asteroid.h"
#include "Bullet.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

enum class EntityType
{
	Base = 1,
	Player = 2,
	Asteroid = 3,
	Bullet = 4
};

#pragma region Smart Structs
// Struct representing a smart entity
struct SmartEntity
{
	// Constructors
	SmartEntity() { }
	SmartEntity(Entity* entity, std::string meshName, std::string materialName) : entity(entity), meshName(meshName), materialName(materialName) { }

	// Members
	Entity* entity; // Entity Pointer
	std::string meshName; // Name of the mesh this entity utilizes
	std::string materialName; // Name of the material this entity utilizes
};

// Struct representing a smart mesh
struct SmartMesh
{
	// Constructors
	SmartMesh() { }
	SmartMesh(Mesh* mesh, unsigned int refCount) : mesh(mesh), refCount(refCount) { }

	// Members
	Mesh* mesh; // Mesh Pointer
	unsigned int refCount; // Number of references to this mesh
};

// Struct representing a smart material
struct SmartMaterial
{
	// Constructors
	SmartMaterial() { }
	SmartMaterial(
		Material* material, 
		unsigned int refCount, 
		std::string vertexShaderName, 
		std::string pixelShaderName, 
		std::string shaderResourceViewBaseColorName,
		std::string shaderResourceViewNormalName,
		std::string samplerStateName
	) :
		material(material), refCount(refCount), 
		vertexShaderName(vertexShaderName), 
		pixelShaderName(pixelShaderName), 
		shaderResourceViewBaseColorName(shaderResourceViewBaseColorName),
		shaderResourceViewNormalName(shaderResourceViewNormalName),
		samplerStateName(samplerStateName) { }

	// Members
	Material* material; // Material Pointer
	unsigned int refCount; // Number of references to this material
	std::string vertexShaderName; // Name of the vertex shader this material utilizes
	std::string pixelShaderName; // Name of the pixel shader this material utilizes
	std::string shaderResourceViewBaseColorName; // Name of the shader resource view base color this material utilizes
	std::string shaderResourceViewNormalName; // Name of the shader resource view normal this material utilizes
	std::string samplerStateName; // Name of the sampler state this material utilizes
};

// Struct representing a smart emitter
struct SmartEmitter 
{
	SmartEmitter() {}
	SmartEmitter(Emitter * emitter) : emitter(emitter), refCount(refCount) { }

	// Members
	Emitter * emitter;
	unsigned int refCount;
};
// Struct representing a smart simple vertex shader
struct SmartVertexShader
{
	// Constructors
	SmartVertexShader() { }
	SmartVertexShader(SimpleVertexShader* vertexShader, unsigned int refCount) : vertexShader(vertexShader), refCount(refCount) { }

	// Members
	SimpleVertexShader* vertexShader; // Simple Vertex Shader Pointer
	unsigned int refCount; // Number of references to this material
};

// Struct representing a smart simple pixel shader
struct SmartPixelShader
{
	// Constructors
	SmartPixelShader() { }
	SmartPixelShader(SimplePixelShader* pixelShader, unsigned int refCount) : pixelShader(pixelShader), refCount(refCount) { }

	// Members
	SimplePixelShader* pixelShader; // Simple Pixel Shader Pointer
	unsigned int refCount; // Number of references to this material
};

// Struct representing a smart shader resource view
struct SmartShaderResourceView
{
	// Constructors
	SmartShaderResourceView() { }
	SmartShaderResourceView(ID3D11ShaderResourceView* shaderResourceView, unsigned int refCount) : shaderResourceView(shaderResourceView), refCount(refCount) { }

	// Members
	ID3D11ShaderResourceView* shaderResourceView; // DXTK Shader Reource View Pointer
	unsigned int refCount; // Number of references to this material
};

// Struct representing a smart sampler state
struct SmartSamplerState
{
	// Constructors
	SmartSamplerState() { }
	SmartSamplerState(ID3D11SamplerState* samplerState, unsigned int refCount) : samplerState(samplerState), refCount(refCount) { }

	// Members
	ID3D11SamplerState* samplerState; // DXTK Shader Reource View Pointer
	unsigned int refCount; // Number of references to this material
};
#pragma endregion

class EntityManager
{
public:
	EntityManager(); // Constructor
	~EntityManager(); // Deconstructor

	// Runs the update method on all entities
	// returns a bool if we should change scenes
	// janky and should probably change but for now
	// its ok
	bool UpdateEntities(float deltaTime, float totalTime);

	// Draws all entities with lighting
	void DrawEntities(ID3D11DeviceContext* context, Camera* camera, DirectionalLight lights[], int lightCount, ID3D11ShaderResourceView* skySRV);

	#pragma region Public Helper Methods
	// Entity Helper Methods
	void CreateEntity(std::string entityName, std::string meshName, std::string materialName, EntityType type);
	void CreateEntityWithEmitter(std::string entityName, std::string meshName, std::string materialName, std::string emitterName, EntityType type);
	void RemoveEntity(std::string entityName);
	Entity* GetEntity(std::string entityName);

	// Mesh Helper Methods
	void CreateMesh(std::string meshName, ID3D11Device* device, char* objFile);
	void RemoveMesh(std::string meshName);

	// Material Helper Methods
	void CreateMaterial(std::string materialName, std::string vertexShaderName, std::string pixelShaderName, std::string shaderResourceViewBaseColorName, std::string samplerStateName);
	void CreateMaterialWithNormal(std::string materialName, std::string vertexShaderName, std::string pixelShaderName, std::string shaderResourceViewBaseColorName, std::string shaderResourceViewNormalName, std::string samplerStateName);
	void RemoveMaterial(std::string materialName);

	// Vertex Shader Helper Methods
	void CreateVertexShader(std::string vertexShaderName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile);
	void RemoveVertexShader(std::string vertexShaderName);

	// Vertex Shader Helper Methods
	void CreatePixelShader(std::string pixelShaderName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile);
	void RemovePixelShader(std::string pixelShaderName);

	// Shader Resource View Helper Methods
	void CreateShaderResourceView(std::string shaderResourceViewName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile);
	void CreateInteriorMappingDDSShaderResourceView(std::string shaderResourceViewName, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR textureFile);
	void RemoveShaderResourceView(std::string shaderResourceViewName);

	// Sampler State Helper Methods
	void CreateSamplerState(std::string samplerStateName, ID3D11Device* device, D3D11_SAMPLER_DESC samplerDesc);
	void RemoveSamplerState(std::string samplerStateName);

	// Emitter Helper Methods
	void CreateEmitter(std::string emitterName, ID3D11Device* device, std::string vs, std::string ps, std::string texture, ID3D11DepthStencilState* particleDepthState, ID3D11BlendState* particleBlendState);
	void RemoveEmitter(std::string emitterName);
	#pragma endregion

private:
	// Map of all smart entities handled in the manager (Uses entity name for the key)
	std::map<std::string, SmartEntity> entities;

	// Maps to keep track of entity related objects
	std::map<std::string, SmartMesh> meshes; // Smart Meshes Map (Uses mesh name for the key)
	std::map<std::string, SmartEmitter> emitters; // Smart Meshes Map (Uses mesh name for the key)
	std::map<std::string, SmartMaterial> materials; // Smart Materials Map (Uses material name for the key)
	std::map<std::string, SmartVertexShader> vertexShaders; // Smart Vertex Shaders Map (Uses vertex shader name for the key)
	std::map<std::string, SmartPixelShader> pixelShaders; // Smart Pixel Shaders Map (Uses pixel shader name for the key)
	std::map<std::string, SmartShaderResourceView> shaderResourceViews; // Smart Shader Resource Views Map (Uses shader resource view name for the key)
	std::map<std::string, SmartSamplerState> samplerStates; // Smart Sampler States Map (Uses sampler state name for the key)

	#pragma region Private Helper Methods
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

	// Collision detection helper method
	// returns true if collision is found
	bool CheckForCollision(Entity * entity1, Entity * entity2);

	// Counter for asteroids
	int asteroidCount = 0;
	#pragma endregion
};
#endif