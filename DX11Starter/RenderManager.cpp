#include "RenderManager.h"


using namespace DirectX;

RenderManager::RenderManager()
{
}


RenderManager::~RenderManager()
{
	// Clean Up Shadows
	spot1ShadowDSV->Release();
	spot1ShadowSRV->Release();
	spot2ShadowDSV->Release();
	spot2ShadowSRV->Release();
	dirShadowDSV->Release();
	dirShadowSRV->Release();
	shadowRasterizer->Release();
	shadowSampler->Release();
	delete shadowVertexShader;

	// Clean up Particles
	delete partVertexShader;
	delete partPixelShader;
	particleDepthState->Release();
	particleBlendState->Release();

	// Clean up Post-processing
	ppRTV->Release();
	ppSRV->Release();
	delete ppPS;
	delete ppVS;

	screen->release();
}

std::vector<Material*> RenderManager::getMaterials()
{
	return materials;
}

void RenderManager::setSceneData(Camera * cam, DirectionalLight dirLight, DirectionalLight dirLight2, PointLight pointLight, SpotLight spotLight, SpotLight spotLight2)
{
	// Data going to Vertex Shader
	materials[0]->getVertexShader()->SetMatrix4x4("view", cam->getView());
	materials[0]->getVertexShader()->SetMatrix4x4("projection", cam->getProj());
	materials[0]->getVertexShader()->SetMatrix4x4("spot1View", spot1ShadowViewMatrix);
	materials[0]->getVertexShader()->SetMatrix4x4("spot1Proj", spot1ShadowProjectionMatrix);
	materials[0]->getVertexShader()->SetMatrix4x4("spot2View", spot2ShadowViewMatrix);
	materials[0]->getVertexShader()->SetMatrix4x4("spot2Proj", spot2ShadowProjectionMatrix);
	materials[0]->getVertexShader()->SetMatrix4x4("dirView", dirShadowViewMatrix);
	materials[0]->getVertexShader()->SetMatrix4x4("dirProj", dirShadowProjectionMatrix);

	materials[0]->getVertexShader()->CopyAllBufferData();

	// Data going to the Pixel Shader
	bool checkDL = materials[0]->getPixelShader()->SetData(
		"dirLight",
		&dirLight,
		sizeof(DirectionalLight));
	bool checkDL2 = materials[0]->getPixelShader()->SetData(
		"dirLight2",
		&dirLight2,
		sizeof(DirectionalLight));
	bool checkPL = materials[0]->getPixelShader()->SetData(
		"pointLight",
		&pointLight,
		sizeof(PointLight));
	bool checkSL = materials[0]->getPixelShader()->SetData(
		"spotLight",
		&spotLight,
		sizeof(SpotLight));
	bool checkSL2 = materials[0]->getPixelShader()->SetData(
		"spotLight2",
		&spotLight2,
		sizeof(SpotLight));

	materials[0]->getPixelShader()->SetFloat3("cameraPos", cam->getPosition());
	materials[0]->getPixelShader()->SetSamplerState("ShadowSampler", shadowSampler);
	materials[0]->getPixelShader()->SetShaderResourceView("spot1ShadowMap", spot1ShadowSRV);
	materials[0]->getPixelShader()->SetShaderResourceView("spot2ShadowMap", spot2ShadowSRV);
	materials[0]->getPixelShader()->SetShaderResourceView("dirShadowMap", dirShadowSRV);

	materials[0]->getPixelShader()->CopyAllBufferData();
}

void RenderManager::setObjData(Entity * object)
{
	// Data going to Vertex Shader
	object->getMat()->getVertexShader()->SetMatrix4x4("world", object->getWorld());
	object->getMat()->getVertexShader()->CopyAllBufferData();

	// Data going to the Pixel Shader
	object->getMat()->getPixelShader()->SetShaderResourceView("diffuseTexture", object->getMat()->getTexture());
	object->getMat()->getPixelShader()->SetSamplerState("sampState", object->getMat()->getSampler());
	object->getMat()->getPixelShader()->SetShaderResourceView("normalMap", object->getMat()->getNormalMap());

	object->getMat()->getPixelShader()->CopyAllBufferData();
}

SimpleVertexShader * RenderManager::getPartVert()
{
	return partVertexShader;
}

SimplePixelShader * RenderManager::getPartPix()
{
	return partPixelShader;
}

ID3D11ShaderResourceView* RenderManager::getPartText(int index)
{
	return particleTextures[index];
}

void RenderManager::DefaultLastTime()
{
	lastTime = 0;
}

void RenderManager::coinSpinShrink(float totalTime, float radian, Entity * coinObj)
{	
	if (totalTime > lastTime + 0.1f)
	{
		coinObj->setScale(XMFLOAT3(coinObj->getScale().x, coinObj->getScale().y, coinObj->getScale().z));

		XMVECTOR pastRotation = XMVectorSet(coinObj->getRotation().x, coinObj->getRotation().y, coinObj->getRotation().z, 0.f);
		pastRotation = XMVector3Rotate(pastRotation, XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), radian));
		XMFLOAT3 newRotation;
		XMStoreFloat3(&newRotation, pastRotation);
		coinObj->setRotation(newRotation);
	}
}

void RenderManager::moveSpotLights(float deltaTime, SpotLight* spotLight, SpotLight* spotLight2)
{
	(*spotLight).location.x -= 5 * deltaTime;
	XMMATRIX shView = XMMatrixLookToLH(
		XMVectorSet(spotLight->location.x, spotLight->location.y, spotLight->location.z, 0),	// Light position
		XMVectorSet(spotLight->direction.x, spotLight->direction.y, spotLight->direction.z, 0),	// Light direction
		XMVectorSet(0, 0, 1, 0));																// Up direction
	XMStoreFloat4x4(&spot1ShadowViewMatrix, XMMatrixTranspose(shView));
	(*spotLight2).location.x -= 5 * deltaTime;
	shView = XMMatrixLookToLH(
		XMVectorSet(spotLight2->location.x, spotLight2->location.y, spotLight2->location.z, 0),	// Light position
		XMVectorSet(spotLight2->direction.x, spotLight2->direction.y, spotLight2->direction.z, 0),	// Light direction
		XMVectorSet(0, 0, 1, 0));																// Up direction
	XMStoreFloat4x4(&spot2ShadowViewMatrix, XMMatrixTranspose(shView));
}

void RenderManager::loopSpotLights(SpotLight* spotLight, SpotLight* spotLight2, float x)
{
	(*spotLight).location.x = x + 8.5;
	XMMATRIX shView = XMMatrixLookToLH(
		XMVectorSet(spotLight->location.x, spotLight->location.y, spotLight->location.z, 0),	// Light position
		XMVectorSet(spotLight->direction.x, spotLight->direction.y, spotLight->direction.z, 0),	// Light direction
		XMVectorSet(0, 0, 1, 0));																// Up direction
	XMStoreFloat4x4(&spot1ShadowViewMatrix, XMMatrixTranspose(shView));

	(*spotLight2).location.x = x + 18.4;
	shView = XMMatrixLookToLH(
		XMVectorSet(spotLight2->location.x, spotLight2->location.y, spotLight2->location.z, 0),	// Light position
		XMVectorSet(spotLight2->direction.x, spotLight2->direction.y, spotLight2->direction.z, 0),	// Light direction
		XMVectorSet(0, 0, 1, 0));																// Up direction
	XMStoreFloat4x4(&spot2ShadowViewMatrix, XMMatrixTranspose(shView));
}

void RenderManager::rotateSpotLights(int x, int y, int z, float radian, SpotLight* spotLight, SpotLight* spotLight2)
{
	XMVECTOR spotDir = XMVectorSet(spotLight->direction.x, spotLight->direction.y, spotLight->direction.z, 0);
	spotDir = XMVector3Rotate(spotDir, XMQuaternionRotationAxis(XMVectorSet(x, y, z, 0), radian));
	XMStoreFloat3(&(*spotLight).direction, spotDir);
	XMMATRIX shView = XMMatrixLookToLH(
		XMVectorSet(0, 3.5, 0., 0),		// Light position
		spotDir,						// Light direction
		XMVectorSet(0, 0, 1, 0));		// Up direction
	XMStoreFloat4x4(&spot1ShadowViewMatrix, XMMatrixTranspose(shView));

	spotDir = XMVectorSet(spotLight2->direction.x, spotLight2->direction.y, spotLight2->direction.z, 0);
	spotDir = XMVector3Rotate(spotDir, XMQuaternionRotationAxis(XMVectorSet(x, y, z, 0), radian));
	XMStoreFloat3(&(*spotLight2).direction, spotDir);
	shView = XMMatrixLookToLH(
		XMVectorSet(0, 3.5, 0., 0),		// Light position
		spotDir,						// Light direction
		XMVectorSet(0, 0, 1, 0));		// Up direction
	XMStoreFloat4x4(&spot2ShadowViewMatrix, XMMatrixTranspose(shView));
}

void RenderManager::InitShadows(ID3D11Device * device, ID3D11DeviceContext * context, SpotLight* spotLight, SpotLight* spotLight2, DirectionalLight* dirLight)
{
	shadowMapSize = 1024;
	// Spot Light 1 ------------------------------------------------------------------------------------------------
	// Matrices
	XMMATRIX spot1shView = XMMatrixLookToLH(
		XMVectorSet(spotLight->location.x, spotLight->location.y, spotLight->location.z, 0),	// Light position
		XMVectorSet(spotLight->direction.x, spotLight->direction.y, spotLight->direction.z, 0),	// Light direction
		XMVectorSet(0, 0, 1, 0));																// Up direction
	XMStoreFloat4x4(&spot1ShadowViewMatrix, XMMatrixTranspose(spot1shView));
	XMMATRIX spot1shProj = XMMatrixPerspectiveFovLH(
		spotLight->angle * 2,	// Top Down FOV Angle (radians)
		1.0f,					// View Space Aspect Ratio
		0.1f,					// Near clip
		10.0f);					// Far clip
	XMStoreFloat4x4(&spot1ShadowProjectionMatrix, XMMatrixTranspose(spot1shProj));
	// Texture2D
	D3D11_TEXTURE2D_DESC spot1ShadowDesc = {};
	spot1ShadowDesc.Width = shadowMapSize;
	spot1ShadowDesc.Height = shadowMapSize;
	spot1ShadowDesc.ArraySize = 1;
	spot1ShadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	spot1ShadowDesc.CPUAccessFlags = 0;
	spot1ShadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	spot1ShadowDesc.MipLevels = 1;
	spot1ShadowDesc.MiscFlags = 0;
	spot1ShadowDesc.SampleDesc.Count = 1;
	spot1ShadowDesc.SampleDesc.Quality = 0;
	spot1ShadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* spot1ShadowTexture;
	device->CreateTexture2D(&spot1ShadowDesc, 0, &spot1ShadowTexture);
	// DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC spot1ShadowDSDesc = {};
	spot1ShadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	spot1ShadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	spot1ShadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(spot1ShadowTexture, &spot1ShadowDSDesc, &spot1ShadowDSV);
	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC spot1ShadowSRVDesc = {};
	spot1ShadowSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	spot1ShadowSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	spot1ShadowSRVDesc.Texture2D.MipLevels = 1;
	spot1ShadowSRVDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(spot1ShadowTexture, &spot1ShadowSRVDesc, &spot1ShadowSRV);
	// Release the texture reference since we don't need it
	spot1ShadowTexture->Release();
	// Spot Light 1 ------------------------------------------------------------------------------------------------
	// Spot Light 2 ------------------------------------------------------------------------------------------------
	// Matrices
	XMMATRIX shView2 = XMMatrixLookToLH(
		XMVectorSet(spotLight2->location.x, spotLight2->location.y, spotLight2->location.z, 0),	// Light position
		XMVectorSet(spotLight2->direction.x, spotLight2->direction.y, spotLight2->direction.z, 0),	// Light direction
		XMVectorSet(0, 0, 1, 0));																// Up direction
	XMStoreFloat4x4(&spot2ShadowViewMatrix, XMMatrixTranspose(shView2));
	XMMATRIX shProj2 = XMMatrixPerspectiveFovLH(
		spotLight2->angle * 2,	// Top Down FOV Angle (radians)
		1.0f,					// View Space Aspect Ratio
		0.1f,					// Near clip
		10.0f);					// Far clip
	XMStoreFloat4x4(&spot2ShadowProjectionMatrix, XMMatrixTranspose(shProj2));
	// Texture2D
	D3D11_TEXTURE2D_DESC spot2ShadowDesc = {};
	spot2ShadowDesc.Width = shadowMapSize;
	spot2ShadowDesc.Height = shadowMapSize;
	spot2ShadowDesc.ArraySize = 1;
	spot2ShadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	spot2ShadowDesc.CPUAccessFlags = 0;
	spot2ShadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	spot2ShadowDesc.MipLevels = 1;
	spot2ShadowDesc.MiscFlags = 0;
	spot2ShadowDesc.SampleDesc.Count = 1;
	spot2ShadowDesc.SampleDesc.Quality = 0;
	spot2ShadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* spot2ShadowTexture;
	device->CreateTexture2D(&spot2ShadowDesc, 0, &spot2ShadowTexture);
	// DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC spot2ShadowDSDesc = {};
	spot2ShadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	spot2ShadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	spot2ShadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(spot2ShadowTexture, &spot2ShadowDSDesc, &spot2ShadowDSV);
	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC spot2ShadowSRVDesc = {};
	spot2ShadowSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	spot2ShadowSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	spot2ShadowSRVDesc.Texture2D.MipLevels = 1;
	spot2ShadowSRVDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(spot2ShadowTexture, &spot2ShadowSRVDesc, &spot2ShadowSRV);
	// Release the texture reference since we don't need it
	spot2ShadowTexture->Release();
	// Spot Light 1 ------------------------------------------------------------------------------------------------
	// Dir Light ---------------------------------------------------------------------------------------------------
	// Matrices
	XMMATRIX shView3 = XMMatrixLookToLH(
		XMVectorSet(0, 3.5, 0, 0),																// Light position
		XMVectorSet(dirLight->direction.x, dirLight->direction.y, dirLight->direction.z, 0),	// Light direction
		XMVectorSet(1, 0, 0, 0));																// Up direction
	XMStoreFloat4x4(&dirShadowViewMatrix, XMMatrixTranspose(shView3));
	XMMATRIX shProj3 = XMMatrixOrthographicLH(
		2.f,					// View Width
		20.f,					// View Height
		0.1f,					// Near clip
		10.0f);					// Far clip
	XMStoreFloat4x4(&dirShadowProjectionMatrix, XMMatrixTranspose(shProj3));
	// Texture2D
	D3D11_TEXTURE2D_DESC dirShadowDesc = {};
	dirShadowDesc.Width = shadowMapSize;
	dirShadowDesc.Height = shadowMapSize;
	dirShadowDesc.ArraySize = 1;
	dirShadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dirShadowDesc.CPUAccessFlags = 0;
	dirShadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	dirShadowDesc.MipLevels = 1;
	dirShadowDesc.MiscFlags = 0;
	dirShadowDesc.SampleDesc.Count = 1;
	dirShadowDesc.SampleDesc.Quality = 0;
	dirShadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* dirShadowTexture;
	device->CreateTexture2D(&dirShadowDesc, 0, &dirShadowTexture);
	// DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC dirShadowDSDesc = {};
	dirShadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dirShadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dirShadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(dirShadowTexture, &dirShadowDSDesc, &dirShadowDSV);
	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC dirShadowSRVDesc = {};
	dirShadowSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	dirShadowSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	dirShadowSRVDesc.Texture2D.MipLevels = 1;
	dirShadowSRVDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(dirShadowTexture, &dirShadowSRVDesc, &dirShadowSRV);
	// Release the texture reference since we don't need it
	dirShadowTexture->Release();
	// Dir Light ---------------------------------------------------------------------------------------------------

	// special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	// Rasterizer State
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);
}

void RenderManager::RenderSpot1ShadowMap(ID3D11DeviceContext * context, std::vector<Entity*>* gameObjects, ID3D11RenderTargetView * backBufferRTV, ID3D11DepthStencilView * depthStencilView, unsigned int * width, unsigned int * height)
{
	// Set up the render targets and depth buffer (shadow map) and
	// other pipeline states
	context->OMSetRenderTargets(0, 0, spot1ShadowDSV);
	context->ClearDepthStencilView(spot1ShadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadowRasterizer);

	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = (float)shadowMapSize;
	vp.Height = (float)shadowMapSize;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	// Set up the vertex shader for shadow rendering
	shadowVertexShader->SetShader();
	shadowVertexShader->SetMatrix4x4("view", spot1ShadowViewMatrix);
	shadowVertexShader->SetMatrix4x4("projection", spot1ShadowProjectionMatrix);

	// Turn off the pixel shader
	context->PSSetShader(0, 0, 0);

	// Render all of our entities to the shadow map
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for each (Entity* object in *gameObjects)
	{
		// Set buffers in the input assembler
		ID3D11Buffer* vb = object->getMesh()->getVertexBuffer();
		ID3D11Buffer* ib = object->getMesh()->getIndexBuffer();
		context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		shadowVertexShader->SetMatrix4x4("world", object->getWorld());
		shadowVertexShader->CopyAllBufferData();
		
		// Draw into shadow map
		context->DrawIndexed(object->getMesh()->getIndexCount(), 0, 0);
	}

	// Revert all the DX settings for "regular" drawing
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	vp.Width = (float)*width;
	vp.Height = (float)*height;
	context->RSSetViewports(1, &vp);
	context->RSSetState(0); // Restores default state
}

void RenderManager::RenderSpot2ShadowMap(ID3D11DeviceContext * context, std::vector<Entity*>* gameObjects, ID3D11RenderTargetView * backBufferRTV, ID3D11DepthStencilView * depthStencilView, unsigned int * width, unsigned int * height)
{
	// Set up the render targets and depth buffer (shadow map) and
	// other pipeline states
	context->OMSetRenderTargets(0, 0, spot2ShadowDSV);
	context->ClearDepthStencilView(spot2ShadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadowRasterizer);

	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = (float)shadowMapSize;
	vp.Height = (float)shadowMapSize;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	// Set up the vertex shader for shadow rendering
	shadowVertexShader->SetShader();
	shadowVertexShader->SetMatrix4x4("view", spot2ShadowViewMatrix);
	shadowVertexShader->SetMatrix4x4("projection", spot2ShadowProjectionMatrix);

	// Turn off the pixel shader
	context->PSSetShader(0, 0, 0);

	// Render all of our entities to the shadow map
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for each (Entity* object in *gameObjects)
	{
		// Set buffers in the input assembler
		ID3D11Buffer* vb = object->getMesh()->getVertexBuffer();
		ID3D11Buffer* ib = object->getMesh()->getIndexBuffer();
		context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		shadowVertexShader->SetMatrix4x4("world", object->getWorld());
		shadowVertexShader->CopyAllBufferData();

		// Draw into shadow map
		context->DrawIndexed(object->getMesh()->getIndexCount(), 0, 0);
	}

	// Revert all the DX settings for "regular" drawing
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	vp.Width = (float)*width;
	vp.Height = (float)*height;
	context->RSSetViewports(1, &vp);
	context->RSSetState(0); // Restores default state
}

void RenderManager::RenderDirShadowMap(ID3D11DeviceContext * context, std::vector<Entity*>* gameObjects, ID3D11RenderTargetView * backBufferRTV, ID3D11DepthStencilView * depthStencilView, unsigned int * width, unsigned int * height)
{
	// Set up the render targets and depth buffer (shadow map) and
	// other pipeline states
	context->OMSetRenderTargets(0, 0, dirShadowDSV);
	context->ClearDepthStencilView(dirShadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadowRasterizer);

	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = (float)shadowMapSize;
	vp.Height = (float)shadowMapSize;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	// Set up the vertex shader for shadow rendering
	shadowVertexShader->SetShader();
	shadowVertexShader->SetMatrix4x4("view", dirShadowViewMatrix);
	shadowVertexShader->SetMatrix4x4("projection", dirShadowProjectionMatrix);

	// Turn off the pixel shader
	context->PSSetShader(0, 0, 0);

	// Render all of our entities to the shadow map
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for each (Entity* object in *gameObjects)
	{
		// Set buffers in the input assembler
		ID3D11Buffer* vb = object->getMesh()->getVertexBuffer();
		ID3D11Buffer* ib = object->getMesh()->getIndexBuffer();
		context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		shadowVertexShader->SetMatrix4x4("world", object->getWorld());
		shadowVertexShader->CopyAllBufferData();

		// Draw into shadow map
		context->DrawIndexed(object->getMesh()->getIndexCount(), 0, 0);
	}

	// Revert all the DX settings for "regular" drawing
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	vp.Width = (float)*width;
	vp.Height = (float)*height;
	context->RSSetViewports(1, &vp);
	context->RSSetState(0); // Restores default state
}


void RenderManager::LoadShaders(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width, unsigned int height)
{

	//Particle Shader Loading
	partVertexShader = new SimpleVertexShader(device, context);
	partPixelShader = new SimplePixelShader(device, context);


	if (!partVertexShader->LoadShaderFile(L"Debug/PartVertexShader.cso"))
		partVertexShader->LoadShaderFile(L"PartVertexShader.hlsl");

	if (!partPixelShader->LoadShaderFile(L"Debug/PartPixelShader.cso"))
		partPixelShader->LoadShaderFile(L"PartPixelShader.hlsl");

	// Shadow Shader Loading
	shadowVertexShader = new SimpleVertexShader(device, context);
	if (!shadowVertexShader->LoadShaderFile(L"Debug/ShadowVertexShader.cso"))
		shadowVertexShader->LoadShaderFile(L"ShadowVertexShader.hlsl");

	SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.hlsl");

	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	if (!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))
		pixelShader->LoadShaderFile(L"PixelShader.hlsl");

	ppVS = new SimpleVertexShader(device, context);
	if (!ppVS->LoadShaderFile(L"Debug/PostProcessVS.cso"))
		ppVS->LoadShaderFile(L"PostProcessVS.cso");

	ppPS = new SimplePixelShader(device, context);
	if (!ppPS->LoadShaderFile(L"Debug/PostProcessPS.cso"))
		ppPS->LoadShaderFile(L"PostProcessPS.cso");

	ID3D11ShaderResourceView* texture1;
	CreateWICTextureFromFile(device, context, L"textures/sphere.png", 0, &texture1);
	ID3D11ShaderResourceView* texture2;
	CreateWICTextureFromFile(device, context, L"textures/box.png", 0, &texture2);
	ID3D11ShaderResourceView* texture3;
	CreateWICTextureFromFile(device, context, L"textures/disc.png", 0, &texture3);
	ID3D11ShaderResourceView* texture4;
	CreateWICTextureFromFile(device, context, L"textures/rock.jpg", 0, &texture4);;

	// new textures
	ID3D11ShaderResourceView* texture5;
	CreateWICTextureFromFile(device, context, L"textures/INPROGRESS/textures/ceiling.png", 0, &texture5);
	ID3D11ShaderResourceView* texture6;
	CreateWICTextureFromFile(device, context, L"textures/INPROGRESS/textures/wall.png", 0, &texture6);
	ID3D11ShaderResourceView* texture7;
	CreateWICTextureFromFile(device, context, L"textures/INPROGRESS/textures/floor.png", 0, &texture7);
	ID3D11ShaderResourceView* texture8;
	CreateWICTextureFromFile(device, context, L"textures/INPROGRESS/textures/crate.png", 0, &texture8);
	ID3D11ShaderResourceView* texture9;
	CreateWICTextureFromFile(device, context, L"textures/INPROGRESS/textures/coin.png", 0, &texture9);
	ID3D11ShaderResourceView* texture10;
	CreateWICTextureFromFile(device, context, L"textures/INPROGRESS/textures/background.jpg", 0, &texture10);


	ID3D11ShaderResourceView* particleTexture;
	ID3D11ShaderResourceView* particleTexture2;

	CreateWICTextureFromFile(device, context, L"textures/fireParticle.jpg", 0, &particleTexture);
	CreateWICTextureFromFile(device, context, L"textures/particle.jpg", 0, &particleTexture2);

	particleTextures.push_back(particleTexture);
	particleTextures.push_back(particleTexture2);

	textures.push_back(texture1);
	textures.push_back(texture2);
	textures.push_back(texture3);
	textures.push_back(texture4);

	// new textures
	textures.push_back(texture5);
	textures.push_back(texture6);
	textures.push_back(texture7);
	textures.push_back(texture8);
	textures.push_back(texture9);
	textures.push_back(texture10);

	ID3D11ShaderResourceView* normalMap;
	CreateWICTextureFromFile(device, context, L"textures/rockNormals.jpg", 0, &normalMap);

	normalMaps.push_back(normalMap);

	// Sampler state for texture sampling
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sampDesc, &sampler);

	ppSampler = sampler;

	// Create post process resources -----------------------------------------
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* ppTexture;
	device->CreateTexture2D(&textureDesc, 0, &ppTexture);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(ppTexture, &rtvDesc, &ppRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	device->CreateShaderResourceView(ppTexture, &srvDesc, &ppSRV);

	// We don't need the texture reference itself anymore
	ppTexture->Release();

	//Particle depth and blend states
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &particleDepthState);

	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &particleBlendState);

	//Create Emitter for parts

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create and push materials
	materials.push_back(new Material(vertexShader, pixelShader));
	materials[0]->AttatchTexture(textures[0], sampler);
	materials[0]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[1]->AttatchTexture(textures[1], sampler);
	materials[1]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[2]->AttatchTexture(textures[2], sampler);
	materials[2]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[3]->AttatchTexture(textures[3], sampler);
	materials[3]->AttatchNormalMap(normalMaps[0]);

	// new materials
	materials.push_back(new Material(vertexShader, pixelShader));
	materials[4]->AttatchTexture(textures[4], sampler);
	materials[4]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[5]->AttatchTexture(textures[5], sampler);
	materials[5]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[6]->AttatchTexture(textures[6], sampler);
	materials[6]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[7]->AttatchTexture(textures[7], sampler);
	materials[7]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[8]->AttatchTexture(textures[8], sampler);
	materials[8]->AttatchNormalMap(normalMaps[0]);

	materials.push_back(new Material(vertexShader, pixelShader));
	materials[9]->AttatchTexture(textures[9], sampler);
	materials[9]->AttatchNormalMap(normalMaps[0]);
}

void RenderManager::DrawAll(ID3D11DeviceContext * context, std::vector<Entity*> gameObjects, Camera * cam, std::vector<Emitter*> emitters, ID3D11RenderTargetView* backBufferRTV,ID3D11DepthStencilView* depthStencilView, unsigned int width, unsigned int height, std::vector<std::bitset<1>> coinCollected)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Set up the post process render target =======================
	context->OMSetRenderTargets(1, &ppRTV, depthStencilView);
	context->ClearRenderTargetView(ppRTV, color);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	int index = 0;
	for (Entity* object : gameObjects)
	{
		if (index >= 11 && index <= 19)
		{
			if (coinCollected[index - 11].to_ulong() != 1)
			{
				setObjData(object);

				object->getMat()->getVertexShader()->SetShader();

				object->getMat()->getPixelShader()->SetShader();

				// Set buffers in the input assembler
				//  - Do this ONCE PER OBJECT you're drawing, since each object might
				//    have different geometry.
				ID3D11Buffer *verts = object->getMesh()->getVertexBuffer();
				context->IASetVertexBuffers(0, 1, &verts, &stride, &offset);
				context->IASetIndexBuffer(object->getMesh()->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

				context->DrawIndexed(
					object->getMesh()->getIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
					0,     // Offset to the first index we want to use
					0);    // Offset to add to each index when looking up vertices
			}
		}
		else
		{
			setObjData(object);

			object->getMat()->getVertexShader()->SetShader();

			object->getMat()->getPixelShader()->SetShader();

			// Set buffers in the input assembler
			//  - Do this ONCE PER OBJECT you're drawing, since each object might
			//    have different geometry.
			ID3D11Buffer *verts = object->getMesh()->getVertexBuffer();
			context->IASetVertexBuffers(0, 1, &verts, &stride, &offset);
			context->IASetIndexBuffer(object->getMesh()->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

			context->DrawIndexed(
				object->getMesh()->getIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
				0,     // Offset to the first index we want to use
				0);    // Offset to add to each index when looking up vertices
		}
		
		index++;
	}

	//Draw Emitter & parts
	float blend[4] = { 1,1,1,1 };
	context->OMSetBlendState(particleBlendState, blend, 0xffffffff);  // Additive blending
	context->OMSetDepthStencilState(particleDepthState, 0);			// No depth WRITING
	for each (Emitter * e in emitters)
		e->DrawAll(context, cam);
	
	//reset drawstates
	context->OMSetBlendState(0, blend, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);

	// Get ready for post processing ====================
	context->OMSetRenderTargets(1, &backBufferRTV, 0);

	// Turn on VS (no other data necessary)
	ppVS->SetShader();
	ID3D11Buffer *verts = screen->getVertexBuffer();
	context->IASetVertexBuffers(0, 1, &verts, &stride, &offset);
	context->IASetIndexBuffer(screen->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// Turn on PS
	ppPS->SetShader();
	ppPS->SetShaderResourceView("Pixels", ppSRV);
	ppPS->SetSamplerState("Sampler", ppSampler);
	ppPS->SetFloat("pixelWidth", 1.0f / width);
	ppPS->SetFloat("pixelHeight", 1.0f / height);
	ppPS->SetInt("blurAmount", 5);
	ppPS->CopyAllBufferData();

	// Turn off vertex and index buffers
	/*ID3D11Buffer* nothing = 0;
	context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);*/

	context->RSSetState(0);

	// Draw the post process (3 verts = 1 triangle to fill the screen)
	//context->Draw(3, 0);
	context->DrawIndexed(
		screen->getIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices

	// Unbind the post process SRV
	ppPS->SetShaderResourceView("Pixels", 0);
}