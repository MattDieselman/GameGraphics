#include "Emitter.h"



Emitter::Emitter()
{
}


Emitter::~Emitter()
{
	delete[] particles;
	delete[] localPartVerts;
	vertBuffer->Release();
	indexBuffer->Release();
}

Emitter::Emitter(int maxParticles_, int particlesPerSecond_, float lifetime_, float startSize_, float endSize_, DirectX::XMFLOAT4 startColor_, DirectX::XMFLOAT4 endColor_, DirectX::XMFLOAT3 startVelocity_, DirectX::XMFLOAT3 emitterPosition_, DirectX::XMFLOAT3 emitterAcceleration_, ID3D11Device * device_, SimpleVertexShader * vertShader_, SimplePixelShader * pixShader_, ID3D11ShaderResourceView * texture_)
{
	shouldDraw = true;
	vertShader = vertShader_;
	pixShader = pixShader_;
	texture = texture_;

	maxParticles = maxParticles_;
	particlesPerSec = particlesPerSecond_;
	lifetime = lifetime_;
	startSize = startSize_;
	endSize = endSize_;
	startColor = startColor_;
	endColor = endColor_;
	startVel = startVelocity_;
	emitterPos = emitterPosition_;
	emitterAccel = emitterAcceleration_;

	secsPerParticle = 1.0f / particlesPerSecond_;

	timeSinceEmit = 0;
	liveParticles = 0;
	firstAliveIndex = 0;
	firstDeadIndex = 0;

	particles = new Particle[maxParticles];
	localPartVerts = new ParticleVertex[4 * maxParticles];
	for (int i = 0; i < maxParticles * 4; i += 4) {
		localPartVerts[i + 0].UV = DirectX::XMFLOAT2(0, 0);
		localPartVerts[i + 1].UV = DirectX::XMFLOAT2(1, 0);
		localPartVerts[i + 2].UV = DirectX::XMFLOAT2(1, 1);
		localPartVerts[i + 3].UV = DirectX::XMFLOAT2(0, 1);
	}

	D3D11_BUFFER_DESC vertBuffDesc = {};
	vertBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertBuffDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;
	device_->CreateBuffer(&vertBuffDesc, 0, &vertBuffer);

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
	D3D11_BUFFER_DESC indexBuffDesc = {};
	indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffDesc.CPUAccessFlags = 0;
	indexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBuffDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;
	device_->CreateBuffer(&indexBuffDesc, &indexData, &indexBuffer);

	delete[] indices;
}

void Emitter::setPosition(DirectX::XMFLOAT3 position)
{
	emitterPos = position;
}

void Emitter::Update(float dt)
{	
	while (timeSinceEmit > secsPerParticle)
	{
		if(shouldDraw)SpawnPart();
		timeSinceEmit -= secsPerParticle;
	}
	if (firstAliveIndex < firstDeadIndex)
	{
		for (int i = firstAliveIndex; i < firstDeadIndex; i++)
			UpdateSinglePart(dt, i);
	}
	else
	{
		for (int i = firstAliveIndex; i < maxParticles; i++)
			UpdateSinglePart(dt, i);
		for (int i = 0; i < firstDeadIndex; i++)
			UpdateSinglePart(dt, i);
	}


	timeSinceEmit += dt;

}

void Emitter::UpdateSinglePart(float dt, int index)
{
	//if dead, break
	if (particles[index].Age >= lifetime)
		return;
	particles[index].Age += dt;
	
	//Just died,update indexes
	if (particles[index].Age >= lifetime) {
		firstAliveIndex++;
		firstAliveIndex %= maxParticles;
		liveParticles--;
		return;
	}

	//fade between start and end colors
	float ageLerp = particles[index].Age / lifetime;
	DirectX::XMStoreFloat4(&particles[index].Color, DirectX::XMVectorLerp(DirectX::XMLoadFloat4(&startColor), DirectX::XMLoadFloat4(&endColor), ageLerp));
	particles[index].Size = startSize + ageLerp*(endSize - startSize);


	//Move Particles
	DirectX::XMVECTOR startPos = DirectX::XMLoadFloat3(&emitterPos);
	DirectX::XMFLOAT3 temp = particles[index].Position;
	DirectX::XMVECTOR currentPos = DirectX::XMLoadFloat3(&temp);
	DirectX::XMVECTOR startVel = DirectX::XMLoadFloat3(&particles[index].StartVelocity);
	DirectX::XMVECTOR accel = DirectX::XMLoadFloat3(&emitterAccel);
	float t = particles[index].Age;

	//move based on last position
	DirectX::XMStoreFloat3(
		&particles[index].Position,
		accel * t * t / 1.0f + startVel * t + currentPos);
}

void Emitter::SpawnPart()
{
	//If already at max, break
	if (liveParticles >= maxParticles)
		return;

	//recycle old particles
	particles[firstDeadIndex].Age = 0;
	particles[firstDeadIndex].Size = startSize;
	particles[firstDeadIndex].Color = startColor;
	particles[firstDeadIndex].Position = emitterPos;
	particles[firstDeadIndex].StartVelocity = startVel;

	//give some randomness to all the parts
	particles[firstDeadIndex].StartVelocity.x += ((float)rand() / RAND_MAX) * 0.1f - 0.09f;
	particles[firstDeadIndex].StartVelocity.y += ((float)rand() / RAND_MAX) * 0.1f - 0.09f;
	particles[firstDeadIndex].StartVelocity.z += ((float)rand() / RAND_MAX) * 0.1f - 0.09f;

	firstDeadIndex++;
	firstDeadIndex %= maxParticles;
	liveParticles++;
}

void Emitter::CopyPartsToGPU(ID3D11DeviceContext* context)
{
	if (firstAliveIndex < firstDeadIndex)
	{
		for (int i = firstAliveIndex; i < firstDeadIndex; i++)
			CopyPart(i);
	}
	else
	{
		for (int i = firstAliveIndex; i < maxParticles; i++)
			CopyPart(i);
		for (int i = 0; i < firstDeadIndex; i++)
			CopyPart(i);
	}

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	context->Map(vertBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, localPartVerts, sizeof(ParticleVertex) * 4 * maxParticles);
	context->Unmap(vertBuffer, 0);
}

void Emitter::CopyPart(int index)
{
	int i = index * 4;
	localPartVerts[i].Position = particles[index].Position;
	localPartVerts[i+1].Position = particles[index].Position;
	localPartVerts[i+2].Position = particles[index].Position;
	localPartVerts[i+3].Position = particles[index].Position;

	localPartVerts[i].Size = particles[index].Size;
	localPartVerts[i + 1].Size = particles[index].Size;
	localPartVerts[i + 2].Size = particles[index].Size;
	localPartVerts[i + 3].Size = particles[index].Size;

	localPartVerts[i].Color = particles[index].Color;
	localPartVerts[i + 1].Color = particles[index].Color;
	localPartVerts[i + 2].Color = particles[index].Color;
	localPartVerts[i + 3].Color = particles[index].Color;

}

void Emitter::DrawAll(ID3D11DeviceContext * context, Camera * camera)
{
	/*
	if (!shouldDraw) {
		timeSinceEmit = 0;
		liveParticles = 0;
		firstAliveIndex = 0;
		firstDeadIndex = 1;
		return;
	}//*/
	CopyPartsToGPU(context);
	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	vertShader->SetMatrix4x4("view", camera->getView());
	vertShader->SetMatrix4x4("projection", camera->getProj());
	vertShader->SetShader();
	vertShader->CopyAllBufferData();

	pixShader->SetShaderResourceView("particle", texture);
	pixShader->SetShader();
	pixShader->CopyAllBufferData();

	if (firstAliveIndex < firstDeadIndex)
	{
		context->DrawIndexed(liveParticles * 6, firstAliveIndex * 6, 0);
	}
	else
	{
		context->DrawIndexed(firstDeadIndex * 6, 0, 0);
		context->DrawIndexed((maxParticles - firstAliveIndex) * 6, firstAliveIndex * 6, 0);

	}
	
}
