#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "SimpleShader.h"

struct Particle
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 StartVelocity;
	float Size;
	float Age;
};

struct ParticleVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT4 Color;
	float Size;
};

class Emitter
{
public:
	Emitter(
		ID3D11Device* device,
		SimpleVertexShader* vs,
		SimplePixelShader* ps,
		ID3D11ShaderResourceView* texture,
		ID3D11DepthStencilState* particleDepthState,
		ID3D11BlendState* particleBlendState
	);
	~Emitter();

	void Update(float dt);

	void UpdateSingleParticle(float dt, int index);
	void SpawnParticle();

	void CopyParticlesToGPU(ID3D11DeviceContext* context);
	void CopyOneParticle(int index);
	void Draw(ID3D11DeviceContext* context, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);

	#pragma region setters
	void SetMaxParticles(int _maxParticles);
	void SetParticlesPerSecod(int _particlesPerSecond);
	void SetLifetime(float _lifetime);
	void SetStartSize(float _startSize);
	void SetEndSize(float _endSize);
	void SetStartColor(DirectX::XMFLOAT4 _startColor);
	void SetEndColor(DirectX::XMFLOAT4 _endColor);
	void SetEmitterVelocity(DirectX::XMFLOAT3 _startVelocity);
	void SetEmitterPosition(DirectX::XMFLOAT3 _emitterPosition);
	void SetEmitterAcceleration(DirectX::XMFLOAT3 _emitterAcceleration);
	#pragma endregion

private:
	// Emission properties
	int particlesPerSecond;
	float secondsPerParticle;
	float timeSinceEmit;

	int livingParticleCount;
	float lifetime;

	DirectX::XMFLOAT3 emitterAcceleration;
	DirectX::XMFLOAT3 emitterPosition;
	DirectX::XMFLOAT3 startVelocity;
	DirectX::XMFLOAT4 startColor;
	DirectX::XMFLOAT4 endColor;
	float startSize;
	float endSize;

	// Particle array
	Particle* particles;
	int maxParticles;
	int firstDeadIndex;
	int firstAliveIndex;

	// Rendering
	ParticleVertex* localParticleVertices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* texture;
	SimpleVertexShader* vs;
	SimplePixelShader* ps;

	// Blend and depth states
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;
};

