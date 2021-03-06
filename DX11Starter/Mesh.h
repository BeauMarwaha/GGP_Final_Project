#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include <fstream>
#include "Vertex.h"
#include "Collider.h"

// --------------------------------------------------------
// A small key that only allows entities to directly access
// their colliders. Everything should reach through an 
// entity to access the entity specific collider
// --------------------------------------------------------
struct ColliderKey
{
	friend class Entity;
	ColliderKey() {}
};

// --------------------------------------------------------
// A Mesh class that can take vertex and index data for a 
//  model and create/store the requisite related buffers 
//  and mesh-related data for later use
// --------------------------------------------------------
class Mesh
{
public:
	Mesh(ID3D11Device* device, Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount); // Constructor Overload
	Mesh(ID3D11Device* device, char* objFile); // Constructor Overload
	Mesh(Mesh const& other); // Copy Constructor
	Mesh& operator=(Mesh const& other); // Copy Assignment Operator
	~Mesh(); // Destructor

	// GET methods
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();

	// returns a copy of the original collider, making each collider unique to each entity
	// DO NOT directly edit this
	Collider GetCollider(ColliderKey);
	int GetIndexCount();

private:
	// Helper methods
	void Setup(ID3D11Device* device, Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount);
	void CalculateTangents(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount);

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	// Integer specifying how many indices are in the mesh's index buffer
	int indexCount = 0;


	// A collider for the base geometry of the mesh
	// Think of this as the base collider that entites base their own colliders on
	// Each entity of a given mesh is given a copy of this collider to modify
	// by their own scale
	Collider collider;
};

