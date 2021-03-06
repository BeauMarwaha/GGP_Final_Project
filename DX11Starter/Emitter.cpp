#include "Emitter.h"

using namespace DirectX;

Emitter::Emitter(
	ID3D11Device* device,
	SimpleVertexShader* vs,
	SimplePixelShader* ps,
	ID3D11ShaderResourceView* texture,
	ID3D11DepthStencilState* particleDepthState, 
	ID3D11BlendState* particleBlendState
)
{
	// Save params
	this->vs = vs;
	this->ps = ps;
	this->texture = texture;
	this->particleDepthState = particleDepthState;
	this->particleBlendState = particleBlendState;

	this->maxParticles = 1000;

	timeSinceEmit = 0;
	livingParticleCount = 0;
	firstAliveIndex = 0;
	firstDeadIndex = 0;

	// Make the particle array
	particles = new Particle[maxParticles];

	// Safe to create our UVs here
	// also unroll loop a bit, no sense in doing more iterations than needed
	localParticleVertices = new ParticleVertex[4 * maxParticles];
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		localParticleVertices[i + 0].UV = XMFLOAT2(0, 0);
		localParticleVertices[i + 1].UV = XMFLOAT2(1, 0);
		localParticleVertices[i + 2].UV = XMFLOAT2(1, 1);
		localParticleVertices[i + 3].UV = XMFLOAT2(0, 1);
	}


	// Create buffers for drawing particles
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;
	device->CreateBuffer(&vbDesc, 0, &vertexBuffer);

	// Index buffer data
	unsigned int* indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	// Regular index buffer
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;
	device->CreateBuffer(&ibDesc, &indexData, &indexBuffer);

	delete[] indices;
}


Emitter::~Emitter()
{
	delete[] particles;
	delete[] localParticleVertices;
	vertexBuffer->Release();
	indexBuffer->Release();
}

void Emitter::Update(float dt)
{
	// Update all particles - Check cyclic buffer first
	// i.e always draw first alive before first dead
	if (firstAliveIndex < firstDeadIndex)
	{
		for (int i = firstAliveIndex; i < firstDeadIndex; i++)
			UpdateSingleParticle(dt, i);
	}
	else
	{
		// otherwise our living particles will wrap around, this saves
		// us from making tons of new ones
		// Update first half (from firstAlive to max particles)
		for (int i = firstAliveIndex; i < maxParticles; i++)
			UpdateSingleParticle(dt, i);

		// Update second half (from 0 to first dead)
		for (int i = 0; i < firstDeadIndex; i++)
			UpdateSingleParticle(dt, i);
	}

	// Add to the time
	timeSinceEmit += dt;

	// Enough time to emit?
	while (timeSinceEmit > secondsPerParticle)
	{
		SpawnParticle();
		timeSinceEmit -= secondsPerParticle;
	}
}

void Emitter::UpdateSingleParticle(float dt, int index)
{
	// Check for valid particle age before doing anything
	if (particles[index].Age >= lifetime)
		return;

	// Update and check for death
	particles[index].Age += dt;
	if (particles[index].Age >= lifetime)
	{
		// Recent death, so retire by moving alive count
		firstAliveIndex++;
		firstAliveIndex %= maxParticles;
		livingParticleCount--;
		return;
	}

	// Calculate age percentage for lerp
	float agePercent = particles[index].Age / lifetime;

	// Interpolate the color
	XMStoreFloat4(
		&particles[index].Color,
		XMVectorLerp(
			XMLoadFloat4(&startColor),
			XMLoadFloat4(&endColor),
			agePercent));

	// Lerp size
	particles[index].Size = startSize + agePercent * (endSize - startSize);


	// Adjust the position
	XMVECTOR startPos = XMLoadFloat3(&emitterPosition);
	XMVECTOR startVel = XMLoadFloat3(&particles[index].StartVelocity);
	XMVECTOR accel = XMLoadFloat3(&emitterAcceleration);
	float t = particles[index].Age;

	// Use constant acceleration function
	XMStoreFloat3(
		&particles[index].Position,
		accel * t * t / 2.0f + startVel * t + startPos);
}

void Emitter::SpawnParticle()
{
	// Check if there are any particles that need to spawn
	if (livingParticleCount == maxParticles)
		return;

	// Reset the first dead particle
	particles[firstDeadIndex].Age = 0;
	particles[firstDeadIndex].Size = startSize;
	particles[firstDeadIndex].Color = startColor;
	particles[firstDeadIndex].Position = emitterPosition;
	particles[firstDeadIndex].StartVelocity = startVelocity;
	particles[firstDeadIndex].StartVelocity.x += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVelocity.y += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVelocity.z += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;

	// Increment and wrap
	firstDeadIndex++;
	firstDeadIndex %= maxParticles;

	livingParticleCount++;
}

void Emitter::CopyParticlesToGPU(ID3D11DeviceContext* context)
{
	// Update local buffer (living particles only as a speed up)

	// Check cyclic buffer status
	if (firstAliveIndex < firstDeadIndex)
	{
		for (int i = firstAliveIndex; i < firstDeadIndex; i++)
			CopyOneParticle(i);
	}
	else
	{
		// Update first half (from firstAlive to max particles)
		for (int i = firstAliveIndex; i < maxParticles; i++)
			CopyOneParticle(i);

		// Update second half (from 0 to first dead)
		for (int i = 0; i < firstDeadIndex; i++)
			CopyOneParticle(i);
	}

	// All particles copied locally - send whole buffer to GPU
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	memcpy(mapped.pData, localParticleVertices, sizeof(ParticleVertex) * 4 * maxParticles);

	context->Unmap(vertexBuffer, 0);
}

void Emitter::CopyOneParticle(int index)
{
	int i = index * 4;

	localParticleVertices[i + 0].Position = particles[index].Position;
	localParticleVertices[i + 1].Position = particles[index].Position;
	localParticleVertices[i + 2].Position = particles[index].Position;
	localParticleVertices[i + 3].Position = particles[index].Position;

	localParticleVertices[i + 0].Size = particles[index].Size;
	localParticleVertices[i + 1].Size = particles[index].Size;
	localParticleVertices[i + 2].Size = particles[index].Size;
	localParticleVertices[i + 3].Size = particles[index].Size;

	localParticleVertices[i + 0].Color = particles[index].Color;
	localParticleVertices[i + 1].Color = particles[index].Color;
	localParticleVertices[i + 2].Color = particles[index].Color;
	localParticleVertices[i + 3].Color = particles[index].Color;
}

void Emitter::Draw(ID3D11DeviceContext* context, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
{
	float blend[4] = { 1,1,1,1 };
	context->OMSetBlendState(particleBlendState, blend, 0xffffffff);  // Additive blending
	context->OMSetDepthStencilState(particleDepthState, 0);			// No depth WRITING

	// Copy to dynamic buffer
	CopyParticlesToGPU(context);

	// Set up buffers
	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	vs->SetMatrix4x4("view", viewMatrix);
	vs->SetMatrix4x4("projection", projectionMatrix);
	vs->SetShader();
	vs->CopyAllBufferData();

	ps->SetShaderResourceView("particle", texture);
	ps->SetShader();
	ps->CopyAllBufferData();

	// Draw the correct parts of the buffer
	if (firstAliveIndex < firstDeadIndex)
	{
		context->DrawIndexed(livingParticleCount * 6, firstAliveIndex * 6, 0);
	}
	else
	{
		// Draw first half (0 -> dead)
		context->DrawIndexed(firstDeadIndex * 6, 0, 0);

		// Draw second half (alive -> max)
		context->DrawIndexed((maxParticles - firstAliveIndex) * 6, firstAliveIndex * 6, 0);
	}

	// Reset to default states for next frame
	context->OMSetBlendState(0, blend, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);

}

void Emitter::Explode(DirectX::XMFLOAT3 position)
{
	// Update Emitter Position
	emitterPosition = position;

	// Spawn loads of particles
	for (int i = 0; i < maxParticles; i++)
	{
		SpawnExplosionParticle();
	}
}

void Emitter::SpawnExplosionParticle()
{
	// Check if there are any particles that need to spawn
	if (livingParticleCount == maxParticles)
		return;

	// Reset the first dead particle
	particles[firstDeadIndex].Age = 0;
	particles[firstDeadIndex].Size = startSize;
	particles[firstDeadIndex].Color = startColor;
	particles[firstDeadIndex].Position = emitterPosition;
	particles[firstDeadIndex].StartVelocity = XMFLOAT3(rand() % 10 + (-5), rand() % 10 + (-5), rand() % 10 + (-5));
	particles[firstDeadIndex].StartVelocity.x += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVelocity.y += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVelocity.z += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;

	// Increment and wrap
	firstDeadIndex++;
	firstDeadIndex %= maxParticles;

	livingParticleCount++;
}

DirectX::XMFLOAT3 Emitter::GetEmitterPosition()
{
	return emitterPosition;
}

void Emitter::SetMaxParticles(int _maxParticles)
{
	maxParticles = _maxParticles;
}

void Emitter::SetParticlesPerSecod(int _particlesPerSecond)
{
	particlesPerSecond = _particlesPerSecond;
	secondsPerParticle = 1.0f / particlesPerSecond;
}

void Emitter::SetLifetime(float _lifetime)
{
	lifetime = _lifetime;
}

void Emitter::SetStartSize(float _startSize)
{
	startSize = _startSize;
}

void Emitter::SetEndSize(float _endSize)
{
	endSize = _endSize;
}

void Emitter::SetStartColor(DirectX::XMFLOAT4 _startColor)
{
	startColor = _startColor;
}

void Emitter::SetEndColor(DirectX::XMFLOAT4 _endColor)
{
	endColor = _endColor;
}

void Emitter::SetEmitterVelocity(DirectX::XMFLOAT3 _startVelocity)
{
	startVelocity = _startVelocity;
}

void Emitter::SetEmitterPosition(DirectX::XMFLOAT3 _emitterPosition)
{
	emitterPosition = _emitterPosition;
}

void Emitter::SetEmitterAcceleration(DirectX::XMFLOAT3 _emitterAcceleration)
{
	emitterAcceleration = _emitterAcceleration;
}
