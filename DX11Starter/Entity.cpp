#include "Entity.h"

// For the DirectX Math library
using namespace DirectX;

Entity::Entity(Mesh* mesh, Material* material)
{
	// Use the passed in mesh and material
	this->mesh = mesh;
	this->material = material;

	// Set the location vectors and matrix to default values
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	worldMatrix = GetIdentityMatrix();
}

Entity::Entity(Entity const & other)
{
	mesh = other.mesh;
	material = other.material;
	position = other.position;
	rotation = other.rotation;
	scale = other.scale;
	worldMatrix = other.worldMatrix;
}

Entity & Entity::operator=(Entity const & other)
{
	if (this != &other)
	{
		// Switch values
		mesh = other.mesh;
		material = other.material;
		position = other.position;
		rotation = other.rotation;
		scale = other.scale;
		worldMatrix = other.worldMatrix;
	}
	return *this;
}

Entity::~Entity()
{
}

void Entity::Update(float deltaTime, float totalTime)
{
	// Update the world matrix based on the position, rotation, and scale
	XMStoreFloat4x4(&worldMatrix,
		XMMatrixTranslation(position.x, position.y, position.z) *
		XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		XMMatrixScaling(scale.x, scale.y, scale.z));
}

XMFLOAT4X4 Entity::GetWorldMatrix()
{
	return worldMatrix;
}

XMFLOAT3 Entity::GetPosition()
{
	return position;
}

XMFLOAT3 Entity::GetRotation()
{
	return rotation;
}

XMFLOAT3 Entity::GetScale()
{
	return scale;
}

Mesh* Entity::GetMesh()
{
	return mesh;
}

void Entity::SetWorldMatrix(XMFLOAT4X4 worldMatrix)
{
	this->worldMatrix = worldMatrix;
}

void Entity::SetPosition(XMFLOAT3 position)
{
	this->position = position;
}

void Entity::SetRotation(XMFLOAT3 rotation)
{
	this->rotation = rotation;
}

void Entity::SetScale(XMFLOAT3 scale)
{
	this->scale = scale;
}

void Entity::SetMesh(Mesh* mesh)
{
	this->mesh = mesh;
}

void Entity::Move(XMFLOAT3 direction, XMFLOAT3 velocity)
{
	XMVECTOR initialPos = XMLoadFloat3(&position);
	XMVECTOR movement = XMVector3Rotate(XMLoadFloat3(&velocity), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&direction)));
	XMStoreFloat3(&position, initialPos + movement);
}

void Entity::MoveForward(XMFLOAT3 velocity)
{
	XMVECTOR initialPos = XMLoadFloat3(&position);
	XMVECTOR movement = XMVector3Rotate(XMLoadFloat3(&velocity), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)));
	XMStoreFloat3(&position, initialPos + movement);
}

XMFLOAT4X4 Entity::GetIdentityMatrix()
{
	XMFLOAT4X4 identityMatrix = XMFLOAT4X4();
	XMStoreFloat4x4(&identityMatrix, XMMatrixIdentity());
	return identityMatrix;
}

void Entity::Draw(ID3D11DeviceContext* context, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
{
	// Prepare the entity's material
	PrepareMaterial(viewMatrix, projectionMatrix);

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* vBuffer = GetMesh()->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	context->IASetIndexBuffer(GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	//  - Do this ONCE PER OBJECT you intend to draw
	//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
	//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
	//     vertices in the currently set VERTEX BUFFER
	context->DrawIndexed(
		GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}

void Entity::PrepareMaterial(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
{
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.

	// Send the world, view, and projection matrices to the vertex shader
	material->GetVertexShader()->SetMatrix4x4("view", viewMatrix);
	material->GetVertexShader()->SetMatrix4x4("projection", projectionMatrix);
	XMFLOAT4X4 worldMatrixTranspose;
	XMStoreFloat4x4(&worldMatrixTranspose, XMMatrixTranspose(XMLoadFloat4x4(&GetWorldMatrix())));
	material->GetVertexShader()->SetMatrix4x4("world", worldMatrixTranspose);

	// Send the texture information to the pixel shader
	material->GetPixelShader()->SetSamplerState("samplerState", material->GetSamplerState());
	material->GetPixelShader()->SetShaderResourceView("textureBaseColor", material->GetShaderResourceView());

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	material->GetVertexShader()->CopyAllBufferData();
	material->GetPixelShader()->CopyAllBufferData(); 

	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();
}
